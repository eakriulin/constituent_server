#ifndef CONSTITUENT_MIGRATION_H
#define CONSTITUENT_MIGRATION_H

#include <vector>
#include <string>
#include <functional>
#include <pqxx/pqxx>

class Migration {
public:
    static std::vector<Migration> registry;

    std::string name;
    std::function<void(pqxx::work&)> run;

    Migration(std::string&& name, std::function<void(pqxx::work&)>&& run);
};


#endif //CONSTITUENT_MIGRATION_H