#ifndef CONSTITUENT_UTILS_CONTEXT_H
#define CONSTITUENT_UTILS_CONTEXT_H

#include <optional>
#include "httplib.h"

namespace request_context {
    struct Context {
        std::string request_id;
        std::string request_ip;
        std::string request_method;
        std::string request_path;
        std::string request_params;
        std::string request_body;
        int user_id;
    };

    void set_defaults(const httplib::Request &req);

    void set_user_id(int id);
    int get_user_id();

    std::string to_string();
}

#endif //CONSTITUENT_UTILS_CONTEXT_H