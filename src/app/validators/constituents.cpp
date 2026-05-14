#include "constituents.h"
#include "../errors/http_error.h"
#include "../utils/log.h"

ListConstituentDto ConstituentsValidator::to_list_dto(const httplib::Request &req) const {
    try {
        ListConstituentDto dto;

        for (const auto& [key, value] : req.params) {
            if (key == "after") {
                dto.after = value;
            }
        }

        return std::move(dto);
    } catch (const std::exception& e) {
        auto msg = std::format("constituents_validator.to_list_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request params are invalid");
    }
}

AddConstituentDto ConstituentsValidator::to_add_dto(const httplib::Request &req) const {
    AddConstituentDto dto;

    try {
        const nlohmann::json body = nlohmann::json::parse(req.body);
        dto = body.get<AddConstituentDto>();
    } catch (const std::exception& e) {
        auto msg = std::format("constituents_validator.to_create_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }

    if (!this->is_valid_value(dto.value) || !this->is_valid_meaning(dto.meaning) || !this->has_valid_examples(dto.examples)) {
        throw HttpError(400, "Request body is invalid");
    }

    return std::move(dto);
}

EditConstituentDto ConstituentsValidator::to_edit_dto(const httplib::Request &req) const {
    const auto public_id = req.path_params.at("public_id");
    if (public_id.empty()) {
        throw HttpError(400, "Request params are invalid");
    }

    EditConstituentDto dto;

    try {
        const nlohmann::json body = nlohmann::json::parse(req.body);
        dto = body.get<EditConstituentDto>();
        dto.public_id = public_id;
    } catch (const std::exception& e) {
        auto msg = std::format("constituents_validator.to_update_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }

    if (
        (dto.value.has_value() && !this->is_valid_value(dto.value.value())) ||
        (dto.meaning.has_value() && !this->is_valid_meaning(dto.meaning.value())) ||
        (dto.examples.has_value() && !this->has_valid_examples(dto.examples.value()))
    ) {
        throw HttpError(400, "Request body is invalid");
    }

    return std::move(dto);
}

RemoveConstituentDto ConstituentsValidator::to_remove_dto(const httplib::Request &req) const {
    const auto public_id = req.path_params.at("public_id");
    if (public_id.empty()) {
        throw HttpError(400, "Request params are invalid");
    }

    return  RemoveConstituentDto{public_id};
}

EnrichConstituentDto ConstituentsValidator::to_enrich_dto(const httplib::Request &req) const {
    EnrichConstituentDto dto;

    try {
        const nlohmann::json body = nlohmann::json::parse(req.body);
        dto = body.get<EnrichConstituentDto>();
    } catch (const std::exception& e) {
        auto msg = std::format("constituents_validator.to_enrich_dto — {}", e.what());
        log::warn(std::move(msg));

        throw HttpError(400, "Request body is invalid");
    }

    if (!this->is_valid_value(dto.value)) {
        throw HttpError(400, "Request body is invalid");
    }

    return std::move(dto);
}

bool ConstituentsValidator::is_valid_value(const std::string &value) const {
    return !value.empty() && value.size() <= this->MAX_VALUE_SIZE;
}

bool ConstituentsValidator::is_valid_meaning(const std::string &meaning) const {
    return !meaning.empty() && meaning.size() <= this->MAX_MEANING_SIZE;
}

bool ConstituentsValidator::has_valid_examples(const std::vector<ConstituentExample> &examples) const {
    return examples.size() == this->N_EXAMPLES
        && this->is_valid_example(examples.at(0))
        && this->is_valid_example(examples.at(1));
}

bool ConstituentsValidator::is_valid_example(const ConstituentExample &example) const {
    return !example.value.empty()
        && !example.translation.empty()
        && example.value.size() <= this->MAX_EXAMPLE_CONTENT_SIZE
        && example.translation.size() <= this->MAX_EXAMPLE_CONTENT_SIZE;
}
