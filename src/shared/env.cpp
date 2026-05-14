#include <cstdlib>
#include <format>
#include <fstream>
#include "env.h"

static Env env;
static bool is_initialized = false;

int get_int(const char* value, std::string &&name) {
    try {
        return std::stoi(value);
    } catch (const std::invalid_argument&) {
        throw std::runtime_error(std::format("{} must be a valid integer", name));
    }
}

Env &get_env() {
    if (is_initialized) {
        return env;
    }

    if (const auto env_db_username = getenv("DB_USERNAME")) {
        env.db_username = env_db_username;
    } else {
        throw std::runtime_error("DB_USERNAME environment variable is not set");
    }

    if (const auto env_db_password = getenv("DB_PASSWORD")) {
        env.db_password = env_db_password;
    } else {
        throw std::runtime_error("DB_PASSWORD environment variable is not set");
    }

    if (const auto env_db_host = getenv("DB_HOST")) {
        env.db_host = env_db_host;
    } else {
        throw std::runtime_error("DB_HOST environment variable is not set");
    }

    if (const auto env_db_port = getenv("DB_PORT")) {
        env.db_port = env_db_port;
    } else {
        throw std::runtime_error("DB_PORT environment variable is not set");
    }

    if (const auto env_db_name = getenv("DB_NAME")) {
        env.db_name = env_db_name;
    } else {
        env.db_name = "constituent";
    }

    if (const auto env_app_host = getenv("APP_HOST")) {
        env.app_host = env_app_host;
    } else {
        env.app_host = "0.0.0.0";
    }

    if (const auto env_app_port = getenv("APP_PORT")) {
        env.app_port = get_int(env_app_port, "APP_PORT");
    } else {
        env.app_port = 8080;
    }

    if (const auto env_pool_size = getenv("POOL_SIZE")) {
        env.pool_size = get_int(env_pool_size, "POOL_SIZE");
    } else {
        env.pool_size = 1;
    }

    if (const auto env_queue_size = getenv("QUEUE_SIZE")) {
        env.queue_size = get_int(env_queue_size, "QUEUE_SIZE");
    } else {
        env.queue_size = 1000;
    }

    if (const auto env_jwt_secret = getenv("JWT_SECRET")) {
        env.jwt_secret = env_jwt_secret;
    } else {
        throw std::runtime_error("JWT_SECRET environment variable is not set");
    }

    if (const auto env_allowed_origin = getenv("ALLOWED_ORIGIN")) {
        env.allowed_origin = env_allowed_origin;
    } else {
        env.allowed_origin = "*";
    }

    if (const auto llm_api_key = getenv("OPENAI_API_KEY")) {
        env.llm_api_key = llm_api_key;
    } else {
        throw std::runtime_error("OPENAI_API_KEY environment variable is not set");
    }

    if (const auto llm_prompt_id = getenv("OPENAI_PROMPT_ID")) {
        env.llm_prompt_id = llm_prompt_id;
    } else {
        throw std::runtime_error("OPENAI_PROMPT_ID environment variable is not set");
    }

    is_initialized = true;

    return env;
}

void load_dotenv() {
    std::ifstream file(".env");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const auto equals_idx = line.find('=');
        if (equals_idx == std::string::npos) {
            continue;
        }

        auto name = line.substr(0, equals_idx);
        auto value = line.substr(equals_idx + 1);

        setenv(name.c_str(), value.c_str(), 0);
    }
}
