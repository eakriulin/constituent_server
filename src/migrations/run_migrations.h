#ifndef CONSTITUENT_RUN_H
#define CONSTITUENT_RUN_H

#include <string>

void run_migrations(
    const std::string& username,
    const std::string& password,
    const std::string& host,
    const std::string& port,
    const std::string& db_name
);

#endif //CONSTITUENT_RUN_H