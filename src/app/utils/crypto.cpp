#include <algorithm>
#include <vector>
#include <openssl/evp.h>

#include "crypto.h"

static int decode_base64(const std::string& input, std::vector<unsigned char>& out) {
    std::string padded = input;
    if (padded.size() % 4 != 0) {
        const size_t pad = 4 - (padded.size() % 4);
        if (pad > 2) return -1;
        padded.append(pad, '=');
    }
    out.resize((padded.size() / 4) * 3);
    const int written = EVP_DecodeBlock(out.data(),
        reinterpret_cast<const unsigned char*>(padded.data()), padded.size());
    if (written < 0) return -1;
    return written - static_cast<int>(std::count(padded.rbegin(), padded.rend(), '='));
}

bool is_valid_ed25519_public_key(const std::string& base64_key) {
    std::vector<unsigned char> raw;
    if (decode_base64(base64_key, raw) != 32) return false;

    EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, raw.data(), 32);
    if (!pkey) return false;

    EVP_PKEY_free(pkey);
    return true;
}

bool verify_ed25519_signature(const std::string& base64_key, const std::string& message, const std::string& base64_signature) {
    std::vector<unsigned char> pub_raw;
    if (decode_base64(base64_key, pub_raw) != 32) return false;

    std::vector<unsigned char> sig_raw;
    if (decode_base64(base64_signature, sig_raw) != 64) return false;

    EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, pub_raw.data(), 32);
    if (!pkey) return false;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        EVP_PKEY_free(pkey);
        return false;
    }

    if (EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, pkey) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return false;
    }

    const int result = EVP_DigestVerify(ctx,
        sig_raw.data(), 64,
        reinterpret_cast<const unsigned char*>(message.data()), message.size());

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return result == 1;
}