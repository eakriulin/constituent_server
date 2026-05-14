#ifndef CONSTITUENT_UTILS_STRINGS_H
#define CONSTITUENT_UTILS_STRINGS_H

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& s, char separator = '\n');
std::string trim(const std::string& s);
std::string format_response_body(std::string response_body);

#endif //CONSTITUENT_UTILS_STRINGS_H
