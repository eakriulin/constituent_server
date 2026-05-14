#ifndef CONSTITUENT_MIDDLEWARES_ERRORS_H
#define CONSTITUENT_MIDDLEWARES_ERRORS_H

#include "httplib.h"

void errors_middleware(httplib::Server &app);

#endif //CONSTITUENT_MIDDLEWARES_ERRORS_H