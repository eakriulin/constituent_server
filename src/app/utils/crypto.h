#ifndef CONSTITUENT_UTILS_CRYPTO_H
#define CONSTITUENT_UTILS_CRYPTO_H

#include <string>

bool is_valid_ed25519_public_key(const std::string& base64_key);
bool verify_ed25519_signature(const std::string& base64_key, const std::string& message, const std::string& base64_signature);

#endif //CONSTITUENT_UTILS_CRYPTO_H