#include <csignal>
#include "httplib.h"
#include "app/controllers/auth.h"
#include "shared/env.h"
#include "app/db/connection_pool.h"
#include "app/validators/constituents.h"
#include "app/validators/auth.h"
#include "app/repos/constituents.h"
#include "app/repos/auth.h"
#include "app/services/llm.h"
#include "app/services/constituents.h"
#include "app/services/auth.h"
#include "app/services/rate_limits.h"
#include "app/controllers/constituents.h"
#include "app/middlewares/cors_middleware.h"
#include "app/middlewares/errors_middleware.h"
#include "app/middlewares/pre_request_middleware.h"
#include "app/utils/log.h"

int main() {
    load_dotenv();
    const Env& env = get_env();

    const auto connection_string = std::format(
        "postgresql://{}:{}@{}:{}/{}",
        env.db_username,
        env.db_password,
        env.db_host,
        env.db_port,
        env.db_name
    );

    const auto db = std::make_shared<ConnectionPool>(connection_string, env.pool_size);

    const auto auth_validator = std::make_shared<AuthValidator>();
    const auto auth_repo = std::make_shared<AuthRepo>(db);
    const auto auth_service = std::make_shared<AuthService>(auth_repo, env.jwt_secret);

    const auto llm_service = std::make_shared<LLMService>(env.llm_api_key, env.llm_prompt_id);

    const auto constituents_validator = std::make_shared<ConstituentsValidator>();
    const auto constituents_repo = std::make_shared<ConstituentsRepo>(db);
    const auto constituents_service = std::make_shared<ConstituentsService>(constituents_repo, llm_service);

    const auto rate_limits_service = std::make_shared<RateLimitsService>(db);

    const AuthController auth_controller{auth_validator, auth_service, constituents_service};
    const ConstituentsController constituents_controller{constituents_validator, constituents_service, rate_limits_service};

    httplib::Server app;

    static httplib::Server* app_ptr = &app;
    std::signal(SIGINT,  [](int){ app_ptr->stop(); });
    std::signal(SIGTERM, [](int){ app_ptr->stop(); });

    app.new_task_queue = [&env]() {
        return new httplib::ThreadPool(env.pool_size, env.queue_size);
    };

    cors_middleware(app, env.allowed_origin);
    pre_request_middleware(app, env.jwt_secret);
    errors_middleware(app);

    auth_controller.register_routes(app);
    constituents_controller.register_routes(app);

    app.listen(env.app_host, env.app_port);

    log::shutdown();
    return 0;
}
