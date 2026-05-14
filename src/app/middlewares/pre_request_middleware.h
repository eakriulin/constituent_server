#ifndef CONSTITUENT_MIDDLEWARES_CONTEXT_H
#define CONSTITUENT_MIDDLEWARES_CONTEXT_H

#include <string>
#include "httplib.h"

void pre_request_middleware(httplib::Server &app, const std::string &jwt_secret);

#endif //CONSTITUENT_MIDDLEWARES_CONTEXT_H
