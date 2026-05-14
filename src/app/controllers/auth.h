#ifndef CONSTITUENT_CONTROLLERS_AUTH_H
#define CONSTITUENT_CONTROLLERS_AUTH_H

#include <memory>
#include "httplib.h"
#include "../validators/auth.h"
#include "../services/auth.h"
#include "../services/constituents.h"
#include "../types/auth.h"

class AuthController {
private:
    std::shared_ptr<AuthValidator> auth_validator;
    std::shared_ptr<AuthService> auth_service;
    std::shared_ptr<ConstituentsService> constituents_service;
public:
    explicit AuthController(
        std::shared_ptr<AuthValidator> auth_validator,
        std::shared_ptr<AuthService> auth_service,
        std::shared_ptr<ConstituentsService> constituents_service
    );

    void register_routes(httplib::Server& app) const;
};


#endif //CONSTITUENT_CONTROLLERS_AUTH_H