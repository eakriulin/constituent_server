#include "shared/env.h"
#include "./migrations/init_database.h"
#include "./migrations/register_migrations.h"
#include "./migrations/run_migrations.h"

int main() {
    load_dotenv();
    const Env& env = get_env();

    init_database(env.db_username, env.db_password, env.db_host, env.db_port, env.db_name);
    register_migrations();
    run_migrations(env.db_username, env.db_password, env.db_host, env.db_port, env.db_name);

    return 0;
}