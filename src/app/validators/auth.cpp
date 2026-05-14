#include "auth.h"
#include "json.hpp"
#include "../utils/log.h"
#include "../utils/crypto.h"
#include "../errors/http_error.h"

DeviceRegisterDto AuthValidator::to_device_register_dto(const httplib::Request &req) {
    DeviceRegisterDto dto;

    try {
        dto = nlohmann::json::parse(req.body).get<DeviceRegisterDto>();
    } catch (const std::exception& e) {
        auto msg = std::format("auth_validator.to_device_register_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }

    if (!is_valid_ed25519_public_key(dto.public_key)) {
        auto msg = std::format("Invalid ed25519 public key — public_key={}", dto.public_key);
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }

    return std::move(dto);
}

DeviceChallengeDto AuthValidator::to_device_challenge_dto(const httplib::Request &req) {
    try {
        return nlohmann::json::parse(req.body).get<DeviceChallengeDto>();
    } catch (const std::exception& e) {
        auto msg = std::format("auth_validator.to_device_challenge_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }
}

DeviceResponseDto AuthValidator::to_device_response_dto(const httplib::Request &req) {
    try {
        return nlohmann::json::parse(req.body).get<DeviceResponseDto>();
    } catch (const std::exception& e) {
        auto msg = std::format("auth_validator.to_device_response_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }
}
