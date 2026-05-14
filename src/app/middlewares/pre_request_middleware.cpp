#include "pre_request_middleware.h"

#include <unordered_set>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>

#include "../errors/http_error.h"
#include "../utils/log.h"
#include "../utils/request_context.h"

namespace {
    constexpr std::string_view bearer_prefix = "Bearer ";

    const std::unordered_set<std::string> public_routes = {
        "POST /auth/devices/register",
        "POST /auth/devices/challenge",
        "POST /auth/devices/response",
    };

    bool is_public_route(const httplib::Request &req) {
        return req.method == "OPTIONS" || public_routes.contains(req.method + " " + req.path);
    }

    jwt::decoded_jwt<jwt::traits::nlohmann_json> decode_jwt(const std::string& header, const std::string& jwt_secret) {
        if (header.size() <= bearer_prefix.size() ||
            header.compare(0, bearer_prefix.size(), bearer_prefix) != 0) {
            throw HttpError(401, "Invalid header");
        }

        try {
            const auto token = header.substr(bearer_prefix.size());

            const auto decoded_jwt = jwt::decode(token);

            jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
                .verify(decoded_jwt);

            return std::move(decoded_jwt);
        } catch (const std::exception&) {
            throw HttpError(401, "Invalid token");
        }
    }

    void enrich_request_context(jwt::decoded_jwt<jwt::traits::nlohmann_json>&& decoded_jwt) {
        try {
            const auto subject_user_id = decoded_jwt.get_subject();
            const auto user_id = std::stoi(subject_user_id);
            request_context::set_user_id(user_id);
        } catch (const std::exception&) {
            throw HttpError(401, "Invalid token");
        }
    }

    bool process_jwt(const httplib::Request &req, const std::string &jwt_secret) {
        const auto header = req.get_header_value("Authorization");
        if (header.empty()) {
            return false;
        };

        auto decoded_jwt = decode_jwt(header, jwt_secret);

        enrich_request_context(std::move(decoded_jwt));

        return true;
    }
}

void pre_request_middleware(httplib::Server &app, const std::string &jwt_secret) {
    app.set_pre_request_handler([jwt_secret](auto& req, auto& res) -> httplib::Server::HandlerResponse {
        request_context::set_defaults(req);
        auto is_authenticated = process_jwt(req, jwt_secret);

        if (!is_public_route(req) && !is_authenticated) {
            throw HttpError(401, "Unauthenticated");
        }

        auto msg = std::format("{} {}", req.method, req.path);
        log::info(std::move(msg));

        return httplib::Server::HandlerResponse::Unhandled;
    });
}
