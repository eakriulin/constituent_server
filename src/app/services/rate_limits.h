#ifndef CONSTITUENT_SERVICES_RATE_LIMITS_H
#define CONSTITUENT_SERVICES_RATE_LIMITS_H

#include "../db/connection_pool.h"
#include "../types/rate_limits.h"

class RateLimitsService {
private:
    std::shared_ptr<ConnectionPool> db;
public:
    explicit RateLimitsService(std::shared_ptr<ConnectionPool> db);

    [[nodiscard]] bool hit(const std::string &key, int limit, int per_seconds) const;
    [[nodiscard]] std::string get_standard_response() const;
};

#endif //CONSTITUENT_SERVICES_RATE_LIMITS_H