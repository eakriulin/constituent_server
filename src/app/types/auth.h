#ifndef CONSTITUENT_AUTH_H
#define CONSTITUENT_AUTH_H

#include <string>
#include <optional>
#include "json.hpp"

struct DeviceRegisterDto {
    std::string public_key;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DeviceRegisterDto, public_key);

struct RegisterDeviceResult {
    std::string device_id;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RegisterDeviceResult, device_id);

struct DeviceChallengeDto {
    std::string device_id;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DeviceChallengeDto, device_id);

struct DeviceChallenge {
    std::string challenge;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DeviceChallenge, challenge);

struct DeviceResponseDto {
    std::string device_id;
    std::string response;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DeviceResponseDto, device_id, response);

struct Device {
    std::optional<std::string> public_id;
    std::optional<int> user_id;
    std::optional<std::string> public_key;
    std::optional<std::string> challenge;
    std::optional<std::string> challenged_at;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Device, public_id, user_id, public_key, challenge, challenged_at);

struct LoginResult {
    std::string access_token;
    std::string expires_at;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LoginResult, access_token, expires_at);

#endif //CONSTITUENT_AUTH_H