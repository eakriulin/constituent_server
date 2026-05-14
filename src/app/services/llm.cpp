#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "llm.h"

#include <algorithm>
#include "httplib.h"
#include "json.hpp"
#include "../errors/http_error.h"
#include "../utils/log.h"
#include "../types/llm.h"
#include "../utils/strings.h"

LLMService::LLMService(
    const std::string& api_key,
    const std::string& prompt_id
) : api_key(std::move(api_key)),
    prompt_id(std::move(prompt_id)) {}

Constituent LLMService::enrich(EnrichConstituentDto &&dto) {
    httplib::Client client("https://api.openai.com");
    const std::string path = "/v1/responses";

    const httplib::Headers headers = {
        {"Authorization", std::format("Bearer {}", this->api_key)},
    };

    nlohmann::json body;
    body["model"] = "gpt-5.4-nano";
    body["input"] = dto.value;
    body["prompt"]["id"] = this->prompt_id;
    body["store"] = false;
    body["reasoning"]["effort"] = "medium";
    body["reasoning"]["summary"] = "concise";
    body["text"]["verbosity"] = "low";
    body["text"]["format"]["type"] = "text";

    log::info(std::format(
        "llm_service request | request_path={} request_body={}",
        path, body.dump())
    );

    auto response = this->with_retry([&client, &path, &headers, &body]() -> httplib::Result {
        return client.Post(path, headers, body.dump(), "application/json");
    });

    log::info(std::format(
        "llm_service response | request_path={} response_status={} response_body={}",
        path, response->status, format_response_body(response->body))
    );

    return this->to_constituent(response);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
httplib::Result LLMService::with_retry(const std::function<httplib::Result()> &request) {
    for (int8_t attempt = 0; attempt < 3; attempt++) {
        auto response = request();

        // Retry network errors
        if (!response) {
            switch (response.error()) {
                case httplib::Error::Connection:
                case httplib::Error::ConnectionTimeout:
                case httplib::Error::ConnectionClosed:
                case httplib::Error::Timeout:
                case httplib::Error::Read:
                case httplib::Error::Write:
                case httplib::Error::ProxyConnection:
                case httplib::Error::ResourceExhaustion:
                    log::warn(std::format(
                        "llm_service request failed with network error, retrying | error={} attempt={}",
                        httplib::to_string(response.error()), attempt)
                    );
                    continue;
                default:
                    goto done;
            }
        }

        // Success
        if (response->status == 200) {
            return response;
        }

        // Retry 5xx
        if (response->status >= 500) {
            log::warn(std::format(
                "llm_service request failed with 5xx, retrying | response_status={} response_body={} attempt={}",
                response->status, response->body, attempt)
            );
            continue;
        }

        log::warn(std::format(
            "llm_service request failed | response_status={} response_body={} attempt={}",
            response->status, response->body, attempt)
        );
        break;
    }
    done:

    throw HttpError(500, "Something went wrong");
}

// ReSharper disable once CppMemberFunctionMayBeStatic
Constituent LLMService::to_constituent(httplib::Result &response) {
    auto llm_response = nlohmann::json::parse(response->body).get<LLMResponse>();

    std::vector<LLMResponseContentItem>* llm_content = nullptr;
    for (auto&[type, content] : llm_response.output) {
        if (type == "message") {
            llm_content = &content;
        }
    }

    if (llm_content == nullptr || llm_content->size() != 1) {
        log::error(std::format(
            "malformed llm_service response | response_body={}",
            response->body)
        );
        throw HttpError(500, "Something went wrong");
    }

    auto [text] = llm_content->at(0);
    const auto constituent_parts = split(text, '\n');

    if (constituent_parts.size() != 6) {
        log::error(std::format(
            "malformed constituent_parts | response_text={} constituent_parts_size={}",
            text, constituent_parts.size())
        );
        throw HttpError(500, "Something went wrong");
    }

    return {
        std::nullopt,
        std::nullopt,
        std::nullopt,
        trim(constituent_parts[0]),
        trim(constituent_parts[1]),
        std::vector<ConstituentExample>{
            {trim(constituent_parts[2]), trim(constituent_parts[3])},
            {trim(constituent_parts[4]), trim(constituent_parts[5])}
        }
    };
}
