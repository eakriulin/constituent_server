#include "json.hpp"
#include "./constituents.h"
#include "../middlewares/rate_limit_middleware.h"

ConstituentsController::ConstituentsController(
    std::shared_ptr<ConstituentsValidator> constituents_validator,
    std::shared_ptr<ConstituentsService> constituents_service,
    std::shared_ptr<RateLimitsService> rate_limits_service
) : constituents_validator(std::move(constituents_validator)),
    constituents_service(std::move(constituents_service)),
    rate_limits_service(std::move(rate_limits_service)) {}

void ConstituentsController::register_routes(httplib::Server &app) const {
    app.Post(
        "/constituents/enrich",
        rate_limited(rate_limits_service, 60, 3600,
        [this](const httplib::Request &req, httplib::Response &res) {
            auto dto = this->constituents_validator->to_enrich_dto(req);

            const auto constituent = this->constituents_service->enrich(std::move(dto));
            const nlohmann::json response = constituent;

            res.status = 200;
            res.set_content(response.dump(), "application/json");
        }
    ));

    app.Get("/constituents", [this](const httplib::Request &req, httplib::Response &res) {
        auto dto = this->constituents_validator->to_list_dto(req);

        const auto constituents = this->constituents_service->list(std::move(dto));
        const nlohmann::json response = constituents;

        res.status = 200;
        res.set_content(response.dump(), "application/json");
    });

    app.Post("/constituents", [this](const httplib::Request &req, httplib::Response &res) {
        auto dto = this->constituents_validator->to_add_dto(req);

        const auto constituent = this->constituents_service->add(std::move(dto));
        const nlohmann::json response = constituent;

        res.status = 201;
        res.set_content(response.dump(), "application/json");
    });

    app.Patch("/constituents/:public_id", [this](const httplib::Request &req, httplib::Response &res) {
        auto dto = this->constituents_validator->to_edit_dto(req);

        const auto constituent = this->constituents_service->edit(std::move(dto));
        const nlohmann::json response = constituent;

        res.status = 200;
        res.set_content(response.dump(), "application/json");
    });

    app.Delete("/constituents/:public_id", [this](const httplib::Request &req, httplib::Response &res) {
        auto dto = this->constituents_validator->to_remove_dto(req);

        this->constituents_service->remove(std::move(dto));

        res.status = 204;
    });
}