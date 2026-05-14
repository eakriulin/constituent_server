#ifndef CONSTITUENT_SERVICES_CONSTITUENTS_H
#define CONSTITUENT_SERVICES_CONSTITUENTS_H

#include <vector>
#include "llm.h"
#include "../repos/constituents.h"
#include "../types/constituents.h"

class ConstituentsService {
private:
    std::shared_ptr<ConstituentsRepo> constituents_repo;
    std::shared_ptr<LLMService> llm_service;
public:
    explicit ConstituentsService(std::shared_ptr<ConstituentsRepo> constituents_repo, std::shared_ptr<LLMService> llm_service);

    std::vector<Constituent> list(ListConstituentDto &&dto) const;
    Constituent add(AddConstituentDto &&dto) const;
    Constituent edit(EditConstituentDto &&dto) const;
    void remove(RemoveConstituentDto &&dto) const;
    Constituent enrich(EnrichConstituentDto &&dto) const;
    void remove_all_for_user() const;
};


#endif //CONSTITUENT_SERVICES_CONSTITUENTS_H