#ifndef CONSTITUENT_TYPES_CONSTITUENTS_H
#define CONSTITUENT_TYPES_CONSTITUENTS_H

#include <string>
#include <vector>
#include <optional>
#include "json.hpp"

struct ConstituentExample {
    std::string value;
    std::string translation;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConstituentExample, value, translation);

struct ListConstituentDto {
    std::optional<std::string> after;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ListConstituentDto, after);

struct AddConstituentDto {
    std::string value;
    std::string meaning;
    std::vector<ConstituentExample> examples;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AddConstituentDto, value, meaning, examples);

struct EditConstituentDto {
    std::string public_id;
    std::optional<std::string> value;
    std::optional<std::string> meaning;
    std::optional<std::vector<ConstituentExample>> examples;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(EditConstituentDto, value, meaning, examples);

struct RemoveConstituentDto {
    std::string public_id;
};

struct EnrichConstituentDto {
    std::string value;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnrichConstituentDto, value);

struct Constituent {
    std::optional<int> id;
    std::optional<std::string> public_id;
    std::optional<int> user_id;
    std::optional<std::string> value;
    std::optional<std::string> meaning;
    std::optional<std::vector<ConstituentExample>> examples;
};

inline void to_json(nlohmann::json &json, const Constituent& constituent) {
    if (constituent.id.has_value()) {
        json["id"] = *constituent.id;
    }
    if (constituent.public_id.has_value()) {
        json["public_id"] = *constituent.public_id;
    }
    if (constituent.user_id.has_value()) {
        json["user_id"] = *constituent.user_id;
    }
    if (constituent.value.has_value()) {
        json["value"] = *constituent.value;
    }
    if (constituent.meaning.has_value()) {
        json["meaning"] = *constituent.meaning;
    }
    if (constituent.examples.has_value()) {
        json["examples"] = *constituent.examples;
    }
}

#endif //CONSTITUENT_TYPES_CONSTITUENTS_H