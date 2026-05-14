#include "rate_limits.h"

RateLimitsService::RateLimitsService(std::shared_ptr<ConnectionPool> db) : db(std::move(db)) {}

[[nodiscard]] bool RateLimitsService::hit(const std::string &key, int limit, int per_seconds) const {
    const ConnectionGuard connection_guard{this->db, this->db->acquire()};

    pqxx::nontransaction ntx{*connection_guard.connection};

    try {
        ntx.exec(R"(
            INSERT INTO rate_limits (key, tokens, refilled_at)
            VALUES ($1, $2 - 1, NOW())
            ON CONFLICT (key) DO UPDATE
            SET
                tokens = LEAST(
                    $2,
                    rate_limits.tokens
                           + FLOOR($2 * EXTRACT(EPOCH FROM NOW() - rate_limits.refilled_at) / $3)
                ) - 1,
                refilled_at = (
                    CASE
                        WHEN FLOOR($2 * EXTRACT(EPOCH FROM NOW() - rate_limits.refilled_at) / $3) > 0
                        THEN NOW()
                        ELSE rate_limits.refilled_at
                    END
                )
        )", pqxx::params(key, limit, per_seconds));
    } catch (const pqxx::check_violation&) {
        return false;
    }

    return true;
}

[[nodiscard]] std::string RateLimitsService::get_standard_response() const {
    return R"({"error_message": "Too many requests"})";
}
