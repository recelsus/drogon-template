// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include "ApiKeyValidator.hpp"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>
#include <optional>

static const std::string DB_PATH = "./data/api_keys.sqlite";

namespace api_validator {

    std::optional<std::string> getApiKeyLevel(const std::string &key) {
    if (!std::filesystem::exists(DB_PATH)) {
        std::cerr << "API key DB does not exist.\n";
        return std::nullopt;
    }

    sqlite3 *db;
    if (sqlite3_open(DB_PATH.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open API key DB: " << sqlite3_errmsg(db) << "\n";
        return std::nullopt;
    }

    const char *sql = R"sql(
        SELECT level FROM api_keys
        WHERE key = ?
        AND (expired_at IS NULL OR expired_at > CURRENT_TIMESTAMP)
    )sql";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<std::string> level;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *val = sqlite3_column_text(stmt, 0);
        if (val) {
            level = std::string(reinterpret_cast<const char *>(val));
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return level;
}

} // namespace api_validator

