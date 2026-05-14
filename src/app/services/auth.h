#ifndef CONSTITUENT_SERVICES_AUTH_H
#define CONSTITUENT_SERVICES_AUTH_H

#include <memory>
#include "../types/auth.h"
#include "../repos/auth.h"

class AuthService {
private:
    std::shared_ptr<AuthRepo> auth_repo;
    std::string jwt_secret;
    static std::string generate_challenge();
public:
    AuthService(std::shared_ptr<AuthRepo> auth_repo, const std::string& jwt_secret);

    RegisterDeviceResult register_with_device(DeviceRegisterDto&& dto) const;
    DeviceChallenge generate_device_challenge(DeviceChallengeDto&& dto) const;
    LoginResult verify_device_response(DeviceResponseDto&& dto) const;
    void remove_all_for_user() const;
};

#endif //CONSTITUENT_SERVICES_AUTH_H