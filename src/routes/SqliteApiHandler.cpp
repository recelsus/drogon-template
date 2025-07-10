// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include "../security/SecurityMiddleware.hpp"
#include <drogon/drogon.h>
#include <sqlite3.h>
#include <filesystem>
#include <sstream>

using namespace drogon;

static const std::string USER_DB_DIR = "./user_data";

void registerSqliteApiHandler() {
    app().registerHandler("/api/{db}", [](const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback,
                                          const std::string &db_name) {
        auto [allowed, reason] = security::validateApiRequest(req);
        if (!allowed) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k403Forbidden);
            resp->setBody("Access denied: " + reason);
            return callback(resp);
        }

        if (db_name.size() < 8 || db_name.substr(db_name.size() - 7) != ".sqlite") {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Invalid database file name");
            return callback(resp);
        }

        std::string path = USER_DB_DIR + "/" + db_name;
        if (!std::filesystem::exists(path)) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            resp->setBody("Database not found");
            return callback(resp);
        }

        if (req->method() != Post) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k405MethodNotAllowed);
            resp->setBody("Only POST is allowed");
            return callback(resp);
        }

        std::string sql = std::string(req->getBody());
        if (sql.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Missing SQL query in request body");
            return callback(resp);
        }

        sqlite3 *db;
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            resp->setBody("Failed to open database");
            return callback(resp);
        }

        char *errmsg = nullptr;
        std::ostringstream result;
        bool has_rows = false;

        auto callback_fn = [](void *data, int argc, char **argv, char **col_names) -> int {
            std::ostringstream *res = static_cast<std::ostringstream *>(data);
            for (int i = 0; i < argc; ++i) {
                *res << col_names[i] << ": " << (argv[i] ? argv[i] : "NULL") << "\n";
            }
            *res << "---\n";
            return 0;
        };

        int rc = sqlite3_exec(db, sql.c_str(), callback_fn, &result, &errmsg);
        sqlite3_close(db);

        auto resp = HttpResponse::newHttpResponse();
        if (rc != SQLITE_OK) {
            resp->setStatusCode(k400BadRequest);
            resp->setBody(std::string("SQL error: ") + (errmsg ? errmsg : "unknown error"));
            if (errmsg) sqlite3_free(errmsg);
        } else {
            resp->setStatusCode(k200OK);
            resp->setContentTypeCode(CT_TEXT_PLAIN);
            std::string out = result.str();
            resp->setBody(out.empty() ? "Query OK (no output)" : out);
        }

        callback(resp);
    }, {Post});  // Only allow POST method
}

