#include "rate_limit_middleware.h"
#include <format>
#include "../utils/request_context.h"

httplib::Server::Handler rate_limited(
    std::shared_ptr<RateLimitsService> rate_limits_service,
    int limit,
    int per_seconds,
    httplib::Server::Handler handler
) {
    return [rate_limits_service = std::move(rate_limits_service), limit, per_seconds, handler = std::move(handler)](
        const httplib::Request& req, httplib::Response& res
    ) {
        if (
            const auto key = std::format("{}:{}:{}", req.method, req.path, request_context::get_user_id());
            !rate_limits_service->hit(key, limit, per_seconds)
        ) {
            res.status = 429;
            res.set_content(rate_limits_service->get_standard_response(), "application/json");

            return;
        }

        handler(req, res);
    };
}
