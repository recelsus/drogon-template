#include "RateLimiter.hpp"
#include <sqlite3.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

static const std::string DB_PATH = "./data/rate_limit.sqlite";

namespace rate_limiter {

std::string currentWindowStart(int window_seconds) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto epoch = system_clock::to_time_t(now);
    epoch -= epoch % window_seconds;
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&epoch), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool isAllowed(const std::string &ip, const std::string &endpoint, int max_requests, int window_seconds) {
    sqlite3 *db;
    if (sqlite3_open(DB_PATH.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open rate_limit DB\n";
        return true;
    }

    const std::string window = currentWindowStart(window_seconds);

    const char *select_sql = R"sql(
        SELECT count FROM rate_limits
        WHERE ip = ? AND endpoint = ? AND window_start = ?
    )sql";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, ip.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, endpoint.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, window.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (count >= max_requests) {
        sqlite3_close(db);
        return false;
    }

    const char *upsert_sql = R"sql(
        INSERT INTO rate_limits (ip, endpoint, window_start, count)
        VALUES (?, ?, ?, 1)
        ON CONFLICT(ip, endpoint, window_start)
        DO UPDATE SET count = count + 1
    )sql";

    sqlite3_prepare_v2(db, upsert_sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, ip.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, endpoint.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, window.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    return true;
}

} // namespace rate_limiter

