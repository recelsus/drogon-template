// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include <chrono>
#include <drogon/drogon.h>
#include "../db/AccessLog.hpp"
#include "../security/SecurityMiddleware.hpp"

using namespace drogon;

void registerRootHandler() {
    app().registerHandler("/", [](const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {
        auto start = std::chrono::steady_clock::now();
        bool allowed = false;
        std::string reason;
        try {
            std::tie(allowed, reason) = security::validatePublicRequest(req);

            if (!allowed) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k403Forbidden);
                resp->setContentTypeCode(CT_TEXT_HTML);
                resp->setBody("<h1>Access Denied</h1><p>" + reason + "</p>");

                auto end = std::chrono::steady_clock::now();
                int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                log_db::insert(req, false, reason, duration);

                callback(resp);
                return;
            }

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

            auto end = std::chrono::steady_clock::now();
            int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            log_db::insert(req, false, "Internal Server Error: " + std::string(e.what()), duration);

            callback(resp);
        }
    });
}

