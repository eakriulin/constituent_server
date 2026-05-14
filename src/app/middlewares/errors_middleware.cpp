#include "errors_middleware.h"
#include "../errors/http_error.h"
#include "../utils/log.h"

void errors_middleware(httplib::Server &app) {
    app.set_exception_handler([](const auto& req, auto& res, const std::exception_ptr& ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const HttpError& e) {
            res.status = e.status_code;
            res.set_content(e.to_json_string(), "application/json");

            auto msg = std::format("{} {} {} — {}", e.status_code, req.method, req.path, e.what());
            log::warn(std::move(msg));
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(R"({"error_message":"Internal server error"})", "application/json");

            auto msg = std::format("500 {} {} — {}", req.method, req.path, e.what());
            log::error(std::move(msg));
        }
    });
}