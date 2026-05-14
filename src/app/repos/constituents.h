#ifndef CONSTITUENT_REPOS_CONSTITUENTS_H
#define CONSTITUENT_REPOS_CONSTITUENTS_H

#include "../db/connection_pool.h"
#include "../types/constituents.h"

class ConstituentsRepo {
private:
    std::shared_ptr<ConnectionPool> db;

public:
    explicit ConstituentsRepo(std::shared_ptr<ConnectionPool> db);

    std::vector<Constituent> list(ListConstituentDto&& dto) const;
    Constituent add(AddConstituentDto&& dto) const;
    Constituent edit(EditConstituentDto&& dto) const;
    void remove(RemoveConstituentDto&& dto) const;
    void remove_all_for_user() const;
};


#endif //CONSTITUENT_REPOS_CONSTITUENTS_H