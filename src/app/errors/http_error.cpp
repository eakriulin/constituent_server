#include "http_error.h"

#include <format>

HttpError::HttpError(const int status_code, const std::string &message)
    : std::runtime_error(message), status_code(status_code) {}

[[nodiscard]] std::string HttpError::to_json_string() const {
    return std::format(R"({{ "error_message": "{}" }})", this->what());
}
