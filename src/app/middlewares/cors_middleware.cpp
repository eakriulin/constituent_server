#include "cors_middleware.h"

void cors_middleware(httplib::Server &app, const std::string &allowed_origin) {
    app.Options(".*", [allowed_origin](const httplib::Request&, httplib::Response& res) {
        // res.set_header("Access-Control-Allow-Origin", allowed_origin);
        // res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
        // res.set_header("Access-Control-Allow-Headers", "Authorization, Content-Type");
        res.set_header("Access-Control-Max-Age", "86400");
        res.status = 204;
    });

    app.set_post_routing_handler([allowed_origin](const httplib::Request& req, httplib::Response& res) {
        // if (req.method == "OPTIONS") {
        //     return;
        // }
        res.set_header("Access-Control-Allow-Origin", allowed_origin);
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Authorization, Content-Type");
    });
}
