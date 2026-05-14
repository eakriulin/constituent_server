#ifndef CONSTITUENT_VALIDATORS_AUTH_H
#define CONSTITUENT_VALIDATORS_AUTH_H

#include "httplib.h"
#include "../types/auth.h"

class AuthValidator {
public:
    DeviceRegisterDto to_device_register_dto(const httplib::Request& req);
    DeviceChallengeDto to_device_challenge_dto(const httplib::Request& req);
    DeviceResponseDto to_device_response_dto(const httplib::Request& req);
};

#endif //CONSTITUENT_VALIDATORS_AUTH_H