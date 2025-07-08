// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include "IpControl.hpp"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>

namespace ip_control {

static const std::string DB_PATH = "./data/ip_list.sqlite";

bool checkList(const std::string &ip, const std::string &table) {
    if (!std::filesystem::exists(DB_PATH)) {
        std::cerr << "IP list DB not found: " << DB_PATH << "\n";
        return false;
    }

    sqlite3 *db = nullptr;
    if (sqlite3_open(DB_PATH.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open IP list DB: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    std::string sql = "SELECT 1 FROM " + table + " WHERE ip = ?";
    sqlite3_stmt *stmt = nullptr;
    bool result = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, ip.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = true;
        }
    } else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}

bool isWhitelisted(const std::string &ip) {
    return checkList(ip, "whitelist");
}

bool isBlacklisted(const std::string &ip) {
    return checkList(ip, "blacklist");
}

} // namespace ip_control

