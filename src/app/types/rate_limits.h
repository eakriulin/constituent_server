#ifndef CONSTITUENT_TYPES_RATE_LIMITS_H
#define CONSTITUENT_TYPES_RATE_LIMITS_H

#include <string>

struct RateLimit {
    std::string key;
    int count;
    std::string hit_at;
};

#endif //CONSTITUENT_TYPES_RATE_LIMITS_H