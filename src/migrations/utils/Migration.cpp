#include "Migration.h"


std::vector<Migration> Migration::registry;

Migration::Migration(std::string &&name, std::function<void(pqxx::work &)> &&run)
    : name(std::move(name)), run(std::move(run)) {}
