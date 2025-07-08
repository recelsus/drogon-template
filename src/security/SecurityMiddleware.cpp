// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include "SecurityMiddleware.hpp"
#include "./IpControl.hpp"
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>

#include "config/AllowedOrigins.hpp"
#include "ApiKeyValidator.hpp"

namespace security {

std::pair<bool, std::string> validatePublicRequest(const drogon::HttpRequestPtr &req) {
    const std::string ip = req->getPeerAddr().toIp();
    const std::string endpoint = req->path();
    const std::string method = req->methodString();
    const std::string user_agent = req->getHeader("User-Agent");
    const auto query_params = req->getParameters();
    const auto headers = req->headers();
    const auto timestamp = trantor::Date::now().toFormattedString(true);

    if (ip_control::isBlacklisted(ip)) {
        return {false, "denied: IP in blacklist"}; // Blacklisted IPs are denied by default
    }

    if (ip_control::isWhitelisted(ip)) {
        return {true, "allowed: IP in whitelist"}; // Whitelisted IPs are allowed by default
    }

    return {true, "allowed: no specific rules"}; // Default to allowed if not blacklisted or whitelisted
}

std::pair<bool, std::string> validateApiRequest(const drogon::HttpRequestPtr &req) {
    const std::string ip = req->getPeerAddr().toIp();
    const std::string endpoint = req->path();
    const std::string method = req->methodString();
    const std::string user_agent = req->getHeader("User-Agent");
    const auto query_params = req->getParameters();
    const auto headers = req->headers();
    const auto timestamp = trantor::Date::now().toFormattedString(true);

    const std::string api_key = req->getHeader("X-API-KEY");
    const std::string origin = req->getHeader("Origin");
    const std::string referer = req->getHeader("Referer");

    auto [allowed, reason] = validatePublicRequest(req);
    if (!allowed) {
        return {false, reason}; // If the IP is not allowed, return immediately
    }

    if (api_key.empty()) {
        return {false, "denied: missing API key"}; // API key is required for API requests
    }

    if (!api_validator::isValidApiKey(api_key)) {
        return {false, "denied: invalid API key"}; // If the API key is invalid, deny the request
    }

    bool origin_ok = origin.empty() || config::isAllowedOrigin(origin);
    bool referer_ok = referer.empty() || config::isAllowedOrigin(referer);
    
    if (!origin_ok && !referer_ok) {
        return {false, "denied: invalid origin or referer"}; // If both origin and referer are invalid, deny the request
    }
    return {true, "allowed: API request validated successfully"};
}
} // namespace security

