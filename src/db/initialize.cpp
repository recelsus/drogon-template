// db/initialize.cpp
#include "initialize.hpp"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>

namespace init_db {
static const std::string DB_DIR = "./data";

static void ensureDirectoryExists() {
    if (!std::filesystem::exists(DB_DIR)) {
        std::filesystem::create_directory(DB_DIR);
    }
}

static void createAccessLogTable() {
    sqlite3 *db;
    std::string path = DB_DIR + "/access_log.sqlite";
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open access_log DB\n";
        return;
    }

    const char *sql = R"sql(
        CREATE TABLE IF NOT EXISTS access_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ip TEXT NOT NULL,
            endpoint TEXT NOT NULL,
            method TEXT NOT NULL,
            allowed INTEGER NOT NULL,
            reason TEXT DEFAULT NULL,
            user_agent TEXT DEFAULT NULL,
            duration_ms INTEGER DEFAULT 0,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )sql";

    char *errmsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to create access_log table: " << errmsg << "\n";
        sqlite3_free(errmsg);
    }

    sqlite3_close(db);
}

static void createIpListTables() {
    sqlite3 *db;
    std::string path = DB_DIR + "/ip_list.sqlite";
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open ip_list DB\n";
        return;
    }

    const char *sql = R"sql(
        CREATE TABLE IF NOT EXISTS whitelist ( ip TEXT PRIMARY KEY );
        CREATE TABLE IF NOT EXISTS blacklist ( ip TEXT PRIMARY KEY );
    )sql";

    char *errmsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to create ip_list tables: " << errmsg << "\n";
        sqlite3_free(errmsg);
    }

    sqlite3_close(db);
}

static void createApiKeyTable() {
    sqlite3 *db;
    std::string path = DB_DIR + "/api_keys.sqlite";
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open api_keys DB\n";
        return;
    }

    const char *sql = R"sql(
        CREATE TABLE IF NOT EXISTS api_keys (
            key TEXT PRIMARY KEY,
            description TEXT DEFAULT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            expired_at DATETIME DEFAULT NULL
        );
    )sql";

    char *errmsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errmsg) != SQLITE_OK) {
        std::cerr << "Failed to create api_keys table: " << errmsg << "\n";
        sqlite3_free(errmsg);
    }

    sqlite3_close(db);
}

void initialize() {
    ensureDirectoryExists();
    createAccessLogTable();
    createIpListTables();
    createApiKeyTable();
}
} // namespace init_db

