// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include "ApiKeyValidator.hpp"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>

static const std::string DB_PATH = "./data/api_keys.sqlite";

namespace api_validator {

bool isValidApiKey(const std::string &key) {
    if (!std::filesystem::exists(DB_PATH)) {
        std::cerr << "API key DB does not exist.\n";
        return false;
    }

    sqlite3 *db;
    if (sqlite3_open(DB_PATH.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open API key DB: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    const char *sql = R"sql(
        SELECT COUNT(*) FROM api_keys
        WHERE key = ?
        AND (expired_at IS NULL OR expired_at > CURRENT_TIMESTAMP)
    )sql";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    bool valid = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        valid = (count > 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return valid;
}

} // namespace api_validator

