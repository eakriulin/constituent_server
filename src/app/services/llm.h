#ifndef CONSTITUENT_SERVICES_LLM_H
#define CONSTITUENT_SERVICES_LLM_H

#include "httplib.h"
#include "../types/constituents.h"

class LLMService {
private:
    std::string api_key;
    std::string prompt_id;

    httplib::Result with_retry(const std::function<httplib::Result()>& request);
    Constituent to_constituent(httplib::Result& response);
public:
    explicit LLMService(const std::string& api_key, const std::string& prompt_id);

    [[nodiscard]] Constituent enrich(EnrichConstituentDto&& dto);
};

#endif //CONSTITUENT_SERVICES_LLM_H