#ifndef CONSTITUENT_MIDDLEWARES_CORS_H
#define CONSTITUENT_MIDDLEWARES_CORS_H

#include <string>
#include "httplib.h"

void cors_middleware(httplib::Server &app, const std::string &allowed_origin);

#endif //CONSTITUENT_MIDDLEWARES_CORS_H
