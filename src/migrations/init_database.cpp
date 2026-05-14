#include <filesystem>
#include <pqxx/pqxx>

#include "init_database.h"

void init_database(
    const std::string& username,
    const std::string& password,
    const std::string& host,
    const std::string& port,
    const std::string& db_name
) {
    {
        const auto connection_string = std::format("postgresql://{}:{}@{}:{}/postgres", username, password, host, port);
        auto connection = pqxx::connection(connection_string);
        pqxx::nontransaction ntx{connection};

        pqxx::result existing_db_result = ntx.exec(R"(
            SELECT 1 as exists
            FROM pg_database
            WHERE datname = $1
        )", pqxx::params(db_name));

        if (existing_db_result.empty()) {
            ntx.exec(std::format("CREATE DATABASE {}", connection.quote_name(db_name)));
        }
    }

    {
        const auto connection_string = std::format("postgresql://{}:{}@{}:{}/{}", username, password, host, port, db_name);
        auto connection = pqxx::connection(connection_string);
        pqxx::nontransaction ntx{connection};

        const pqxx::result existing_table_result = ntx.exec(R"(
            SELECT 1 as exists
            FROM pg_tables
            WHERE tablename = '_migrations'
        )");

        if (existing_table_result.empty()) {
            ntx.exec(R"(
                CREATE TABLE IF NOT EXISTS _migrations (
                    id SERIAL PRIMARY KEY,
                    name TEXT UNIQUE NOT NULL,
                    finished_at TIMESTAMP NOT NULL DEFAULT NOW()
                )
            )");
        }
    }
}