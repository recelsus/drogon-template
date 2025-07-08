// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#pragma once
#include <drogon/HttpRequest.h>
#include <string>

namespace security {
    std::pair<bool, std::string> validatePublicRequest(const drogon::HttpRequestPtr &req);
    std::pair<bool, std::string> validateApiRequest(const drogon::HttpRequestPtr &req);
}

