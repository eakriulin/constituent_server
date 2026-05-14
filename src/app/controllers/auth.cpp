#include "auth.h"

AuthController::AuthController(
    std::shared_ptr<AuthValidator> auth_validator,
    std::shared_ptr<AuthService> auth_service,
    std::shared_ptr<ConstituentsService> constituents_service
) : auth_validator(std::move(auth_validator)),
    auth_service(std::move(auth_service)),
    constituents_service(std::move(constituents_service)) {}

void AuthController::register_routes(httplib::Server &app) const {
    app.Post("/auth/devices/register", [this](const httplib::Request& req, httplib::Response &res) {
        auto dto = this->auth_validator->to_device_register_dto(req);

        const auto result = this->auth_service->register_with_device(std::move(dto));
        const nlohmann::json response = result;

        res.status = 200;
        res.set_content(response.dump(), "application/json");
    });

    app.Post("/auth/devices/challenge", [this](const httplib::Request& req, httplib::Response &res) {
        auto dto = this->auth_validator->to_device_challenge_dto(req);

        const auto result = this->auth_service->generate_device_challenge(std::move(dto));
        const nlohmann::json response = result;

        res.status = 200;
        res.set_content(response.dump(), "application/json");
    });

    app.Post("/auth/devices/response", [this](const httplib::Request& req, httplib::Response &res) {
        auto dto = this->auth_validator->to_device_response_dto(req);

        const auto result = this->auth_service->verify_device_response(std::move(dto));
        const nlohmann::json response = result;

        res.status = 200;
        res.set_content(response.dump(), "application/json");
    });

    app.Delete("/auth/me", [this](const httplib::Request& req, httplib::Response& res) {
        this->constituents_service->remove_all_for_user();
        this->auth_service->remove_all_for_user();

        res.status = 204;
    });
}
