#ifndef CONSTITUENT_ENV_H
#define CONSTITUENT_ENV_H

#include <string>

struct Env {
    std::string db_username;
    std::string db_password;
    std::string db_host;
    std::string db_port;
    std::string db_name;
    std::string app_host;
    int app_port;
    int pool_size;
    int queue_size;
    std::string jwt_secret;
    std::string allowed_origin;
    std::string llm_api_key;
    std::string llm_prompt_id;
};

Env &get_env();
void load_dotenv();

#endif //CONSTITUENT_ENV_H