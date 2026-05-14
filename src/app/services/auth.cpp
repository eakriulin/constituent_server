#include "auth.h"

#include <chrono>
#include <format>
#include <vector>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "../utils/crypto.h"
#include "../utils/datetime.h"
#include "../errors/http_error.h"

AuthService::AuthService(std::shared_ptr<AuthRepo> auth_repo, const std::string& jwt_secret)
    : auth_repo(std::move(auth_repo)), jwt_secret(std::move(jwt_secret)) {}

RegisterDeviceResult AuthService::register_with_device(DeviceRegisterDto &&dto) const {
    return std::move(this->auth_repo->register_with_device(std::move(dto)));
}

std::string AuthService::generate_challenge() {
    unsigned char raw[32];
    RAND_bytes(raw, sizeof(raw));

    std::vector<char> encoded(45);
    const int len = EVP_EncodeBlock(reinterpret_cast<unsigned char*>(encoded.data()), raw, sizeof(raw));

    return std::string(encoded.data(), len);
}

DeviceChallenge AuthService::generate_device_challenge(DeviceChallengeDto &&dto) const {
    if (const auto device = this->auth_repo->get_device(dto.device_id);
        device.challenged_at.has_value()
    ) {
        if (const auto challenged_at = parse_timestamp(device.challenged_at.value());
            std::chrono::system_clock::now() - challenged_at < std::chrono::seconds{50}
        ) {
            return DeviceChallenge{device.challenge.value()};
        }
    }

    const auto challenge = this->generate_challenge();

    this->auth_repo->store_device_challenge(dto.device_id, challenge);

    return DeviceChallenge{challenge};
}

LoginResult AuthService::verify_device_response(DeviceResponseDto &&dto) const {
    const auto device = this->auth_repo->get_device(dto.device_id);

    if (const auto challenged_at = parse_timestamp(device.challenged_at.value());
        std::chrono::system_clock::now() - challenged_at > std::chrono::seconds{60}
    ) {
        throw HttpError(422, "Challenge expired");
    }

    if (!verify_ed25519_signature(device.public_key.value(), device.challenge.value(), dto.response)) {
        throw HttpError(401, "Invalid response");
    }

    this->auth_repo->clear_device_challenge(dto.device_id);

    const auto now = std::chrono::system_clock::now();
    const auto expires_at = now + std::chrono::hours{24 * 7};

    const auto access_token = jwt::create()
        .set_issued_at(now)
        .set_expires_at(expires_at)
        .set_subject(std::to_string(device.user_id.value()))
        .sign(jwt::algorithm::hs256{this->jwt_secret});

    return LoginResult{
        access_token,
        std::format("{:%FT%TZ}", std::chrono::floor<std::chrono::seconds>(expires_at)),
    };
}

void AuthService::remove_all_for_user() const {
    this->auth_repo->remove_all_for_user();
}
