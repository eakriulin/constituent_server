#include "constituents.h"

ConstituentsService::ConstituentsService(
    std::shared_ptr<ConstituentsRepo> constituents_repo,
    std::shared_ptr<LLMService> llm_service
) : constituents_repo(std::move(constituents_repo)),
    llm_service(std::move(llm_service)) {}

std::vector<Constituent> ConstituentsService::list(ListConstituentDto &&dto) const {
    return std::move(this->constituents_repo->list(std::move(dto)));
}

Constituent ConstituentsService::add(AddConstituentDto &&dto) const {
    return std::move(this->constituents_repo->add(std::move(dto)));
}

Constituent ConstituentsService::edit(EditConstituentDto &&dto) const {
    return std::move(this->constituents_repo->edit(std::move(dto)));
}

void ConstituentsService::remove(RemoveConstituentDto &&dto) const {
    this->constituents_repo->remove(std::move(dto));
}

Constituent ConstituentsService::enrich(EnrichConstituentDto &&dto) const {
    return this->llm_service->enrich(std::move(dto));
}

void ConstituentsService::remove_all_for_user() const {
    this->constituents_repo->remove_all_for_user();
}
