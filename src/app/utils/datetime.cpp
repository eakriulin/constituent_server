#include "datetime.h"

#include <ctime>

std::chrono::system_clock::time_point parse_timestamp(const std::string& value) {
    std::tm tm{};
    strptime(value.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    return std::chrono::system_clock::from_time_t(timegm(&tm));
}
