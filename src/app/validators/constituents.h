#ifndef CONSTITUENT_VALIDATORS_CONSTITUENTS_H
#define CONSTITUENT_VALIDATORS_CONSTITUENTS_H

#include "httplib.h"
#include "../types/constituents.h"

class ConstituentsValidator {
private:
    int MAX_VALUE_SIZE = 120;
    int MAX_MEANING_SIZE = 120;
    int N_EXAMPLES = 2;
    int MAX_EXAMPLE_CONTENT_SIZE = 240;

    [[nodiscard]] bool is_valid_value(const std::string& value) const;
    [[nodiscard]] bool is_valid_meaning(const std::string& meaning) const;
    [[nodiscard]] bool has_valid_examples(const std::vector<ConstituentExample>& examples) const;
    [[nodiscard]] bool is_valid_example(const ConstituentExample& example) const;

public:
    [[nodiscard]] ListConstituentDto to_list_dto(const httplib::Request &req) const;
    [[nodiscard]] AddConstituentDto to_add_dto(const httplib::Request &req) const;
    [[nodiscard]] EditConstituentDto to_edit_dto(const httplib::Request &req) const;
    [[nodiscard]] RemoveConstituentDto to_remove_dto(const httplib::Request &req) const;
    [[nodiscard]] EnrichConstituentDto to_enrich_dto(const httplib::Request &req) const;
};


#endif //CONSTITUENT_VALIDATORS_CONSTITUENTS_H