#ifndef CONSTITUENT_CONTROLLERS_CONSTITUENTS_H
#define CONSTITUENT_CONTROLLERS_CONSTITUENTS_H

#include <memory>
#include "httplib.h"
#include "../validators/constituents.h"
#include "../services/constituents.h"
#include "../services/rate_limits.h"

class ConstituentsController {
private:
    std::shared_ptr<ConstituentsValidator> constituents_validator;
    std::shared_ptr<ConstituentsService> constituents_service;
    std::shared_ptr<RateLimitsService> rate_limits_service;

public:
    explicit ConstituentsController(
        std::shared_ptr<ConstituentsValidator> constituents_validator,
        std::shared_ptr<ConstituentsService> constituents_service,
        std::shared_ptr<RateLimitsService> rate_limits_service
    );

    void register_routes(httplib::Server &app) const;
};


#endif //CONSTITUENT_CONTROLLERS_CONSTITUENTS_H