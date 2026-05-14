#ifndef CONSTITUENT_MIDDLEWARES_RATE_LIMIT_H
#define CONSTITUENT_MIDDLEWARES_RATE_LIMIT_H

#include <memory>
#include "httplib.h"
#include "../services/rate_limits.h"

httplib::Server::Handler rate_limited(
    std::shared_ptr<RateLimitsService> rate_limits_service,
    int limit,
    int per_seconds,
    httplib::Server::Handler handler
);

#endif //CONSTITUENT_MIDDLEWARES_RATE_LIMIT_H
