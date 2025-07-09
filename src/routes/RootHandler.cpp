// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include <chrono>
#include <drogon/drogon.h>
#include "../db/AccessLog.hpp"

using namespace drogon;

void registerRootHandler() {
    app().registerHandler("/", [](const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {
        try {
            auto start = std::chrono::steady_clock::now();

            auto resp = HttpResponse::newHttpResponse();
            resp->setContentTypeCode(CT_TEXT_HTML);
            resp->setBody("<h1>Hello from Drogon (via RootHandler)</h1>");

            callback(resp);

            auto end = std::chrono::steady_clock::now();
            int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            log_db::insert(req, true, "allowed via default", duration);
        } catch (const std::exception &e) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setContentTypeCode(CT_TEXT_HTML);
            resp->setBody(std::string("Internal Server error: ") + e.what());
            callback(resp);
        }
    });
}

