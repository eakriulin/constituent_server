#include "run_migrations.h"
#include "utils/Migration.h"

void run_migrations(
    const std::string& username,
    const std::string& password,
    const std::string& host,
    const std::string& port,
    const std::string& db_name
) {
    const auto connection_string = std::format("postgresql://{}:{}@{}:{}/{}", username, password, host, port, db_name);
    auto connection = pqxx::connection(connection_string);

    for (const Migration& migration : Migration::registry) {
        pqxx::work tx{connection};

        const pqxx::result existing_result = tx.exec(R"(
            SELECT 1
            FROM _migrations
            WHERE name = $1
        )", pqxx::params(migration.name));

        if (existing_result.empty()) {
            migration.run(tx);
            tx.exec("INSERT INTO _migrations (name) VALUES ($1)", pqxx::params(migration.name));
        }

        tx.commit();
    }
}
