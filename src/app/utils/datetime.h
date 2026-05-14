#ifndef CONSTITUENT_UTILS_DATETIME_H
#define CONSTITUENT_UTILS_DATETIME_H

#include <chrono>
#include <string>

std::chrono::system_clock::time_point parse_timestamp(const std::string& value);

#endif //CONSTITUENT_UTILS_DATETIME_H
