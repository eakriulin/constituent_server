#ifndef CONSTITUENT_TYPES_LLM_H
#define CONSTITUENT_TYPES_LLM_H

#include <string>
#include <vector>
#include "json.hpp"

struct LLMResponseContentItem {
    std::string text;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LLMResponseContentItem, text);

struct LLMResponseOutputItem {
    std::string type;
    std::vector<LLMResponseContentItem> content;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LLMResponseOutputItem, type, content);

struct LLMInputTokensDetails {
    int cached_tokens;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LLMInputTokensDetails, cached_tokens);

struct LLMOutputTokensDetails {
    int reasoning_tokens;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LLMOutputTokensDetails, reasoning_tokens);

struct LLMUsage {
    int input_tokens;
    LLMInputTokensDetails input_tokens_details;
    int output_tokens;
    LLMOutputTokensDetails output_tokens_details;
    int total_tokens;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LLMUsage, input_tokens, input_tokens_details, output_tokens, output_tokens_details, total_tokens);

struct LLMResponse {
    std::string model;
    std::vector<LLMResponseOutputItem> output;
    LLMUsage usage;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LLMResponse, model, output, usage);

#endif //CONSTITUENT_TYPES_LLM_H