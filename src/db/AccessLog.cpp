// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include "AccessLog.hpp"
#include <sqlite3.h>
#include <iostream>
#include <string>

namespace log_db {

static const std::string DB_PATH = "./data/access_log.sqlite";

void insert(const drogon::HttpRequestPtr &req,
            bool allowed,
            const std::string &reason,
            int duration_ms) {
    sqlite3 *db;

    if (sqlite3_open(DB_PATH.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << "\n";
        return;
    }

    const char *sql = R"sql(
        INSERT INTO access_log 
            (ip, endpoint, method, allowed, reason, user_agent, duration_ms)
        VALUES
            (?, ?, ?, ?, ?, ?, ?)
    )sql";

    const std::string ip = req->getPeerAddr().toIp();
    const std::string endpoint = req->getPath();
    const std::string method = req->getMethodString();
    const std::string userAgent = req->getHeader("User-Agent");
    int allowed_int = allowed ? 1 : 0;

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, ip.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, endpoint.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, method.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, allowed_int);
    sqlite3_bind_text(stmt, 5, reason.empty() ? nullptr : reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, userAgent.empty() ? nullptr : userAgent.c_str(), -1, SQLITE_TRANSIENT);
    if (duration_ms >= 0) {
        sqlite3_bind_int(stmt, 7, duration_ms);
    } else {
        sqlite3_bind_null(stmt, 7);
    }

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << "\n";
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    }
}
