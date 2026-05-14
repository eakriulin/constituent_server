#ifndef CONSTITUENT_HTTP_ERROR_H
#define CONSTITUENT_HTTP_ERROR_H

#include <stdexcept>
#include <string>

class HttpError : public std::runtime_error {
public:
    int status_code;

    explicit HttpError(int status_code, const std::string &message);

    [[nodiscard]] std::string to_json_string() const;
};

#endif //CONSTITUENT_HTTP_ERROR_H