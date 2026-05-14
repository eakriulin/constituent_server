#include <random>
#include "request_context.h"

namespace {
    thread_local request_context::Context ctx;
    thread_local std::mt19937_64 prng(std::random_device{}());
}

namespace request_context {
    std::string generate_request_id() {
        return std::format("{:016x}{:016x}", prng(), prng());
    }

    static std::string serialize_params(const httplib::Params &params) {
        std::string result;
        for (const auto &[key, value] : params) {
            if (!result.empty()) {
                result += '&';
            }

            result += key;
            result += '=';
            result += value;
        }
        return result;
    }

    void set_defaults(const httplib::Request &req) {
        ctx.request_id = generate_request_id();
        ctx.request_ip = req.remote_addr;
        ctx.request_method = req.method;
        ctx.request_path = req.path;
        ctx.request_body = req.body;
        ctx.request_params = req.params.empty() ? "" : serialize_params(req.params);
        ctx.user_id = 0;
    }

    void set_user_id(int id) {
        ctx.user_id = std::move(id);
    }

    int get_user_id() {
        return ctx.user_id;
    }

    std::string to_string() {
        return  std::format(
            "request_id={} request_ip={} request_method={} request_path={} request_body={} request_params={} user_id={}",
            ctx.request_id,
            ctx.request_ip,
            ctx.request_method,
            ctx.request_path,
            ctx.request_body,
            ctx.request_params,
            ctx.user_id
        );
    }
}