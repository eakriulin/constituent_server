#include "strings.h"

#include <sstream>
#include <algorithm>
#include <regex>

std::vector<std::string> split(const std::string& s, const char separator) {
    std::vector<std::string> parts;
    std::istringstream stream(s);
    std::string part;

    while (std::getline(stream, part, separator)) {
        parts.push_back(std::move(part));
    }

    return std::move(parts);
}

std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    };

    const auto end = s.find_last_not_of(" \t\r\n");

    return s.substr(start, end - start + 1);
}

std::string format_response_body(std::string response_body) {
    std::ranges::replace(response_body, '\n', ' ');

    response_body = std::regex_replace(response_body, std::regex{R"(\s+)"}, " ");
    response_body = std::regex_replace(response_body, std::regex{R"(\{\s)"}, "{");
    response_body = std::regex_replace(response_body, std::regex{R"(\s\})"}, "}");

    return response_body;
}
