// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#pragma once
#include <string>
#include <drogon/HttpRequest.h>
#include <vector>
#include <tuple>

namespace log_db {
    void insert(
        const drogon::HttpRequestPtr &req,
        bool allowed,
        const std::string &reason = "",
        int duration_ms = -1
    );
    std::vector<std::tuple<std::string, std::string, std::string>> selectRecent(int limit = 100);
}

