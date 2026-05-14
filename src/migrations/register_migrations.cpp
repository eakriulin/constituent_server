#include "register_migrations.h"
#include "utils/Migration.h"

void register_migrations() {
    Migration::registry.emplace_back(
        "001_create_users_table", [](pqxx::work& tx) {
            tx.exec(R"(
                CREATE TABLE IF NOT EXISTS users (
                    id SERIAL PRIMARY KEY,
                    public_id UUID NOT NULL DEFAULT gen_random_uuid(),
                    created_at TIMESTAMP DEFAULT NOW(),
                    updated_at TIMESTAMP DEFAULT NOW()
                )
            )");
        }
    );

    Migration::registry.emplace_back(
        "002_create_devices_table", [](pqxx::work& tx) {
            tx.exec(R"(
                CREATE TABLE IF NOT EXISTS devices (
                    id SERIAL PRIMARY KEY,
                    public_id UUID NOT NULL DEFAULT gen_random_uuid(),
                    user_id INT NOT NULL REFERENCES users(id),
                    public_key VARCHAR(255) NOT NULL,
                    challenge VARCHAR(255) NULL,
                    challenged_at TIMESTAMP NULL,
                    created_at TIMESTAMP DEFAULT NOW(),
                    updated_at TIMESTAMP DEFAULT NOW()
                )
            )");
        }
    );

    Migration::registry.emplace_back(
        "003_create_constituents_table", [](pqxx::work& tx) {
            tx.exec(R"(
                CREATE TABLE IF NOT EXISTS constituents (
                    id SERIAL PRIMARY KEY,
                    public_id UUID NOT NULL DEFAULT gen_random_uuid(),
                    user_id INT NOT NULL REFERENCES users(id),
                    value TEXT NOT NULL,
                    meaning TEXT NOT NULL,
                    examples JSONB NOT NULL,
                    created_at TIMESTAMP DEFAULT NOW(),
                    updated_at TIMESTAMP DEFAULT NOW()
                )
            )");
        }
    );

    Migration::registry.emplace_back(
        "004_create_rate_limits_table", [](pqxx::work& tx) {
            tx.exec(R"(
                CREATE UNLOGGED TABLE IF NOT EXISTS rate_limits (
                    key TEXT PRIMARY KEY,
                    tokens INT NOT NULL CHECK (tokens >= 0),
                    refilled_at TIMESTAMP DEFAULT NOW()
                )
            )");
        }
    );

    std::ranges::sort(
        Migration::registry,
        [](const Migration& a, const Migration& b) { return a.name < b.name; }
    );
}
