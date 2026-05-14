#ifndef CONSTITUENT_000_INIT_H
#define CONSTITUENT_000_INIT_H

#include <string>

void init_database(
    const std::string& username,
    const std::string& password,
    const std::string& host,
    const std::string& port,
    const std::string& db_name
);

#endif //CONSTITUENT_000_INIT_H