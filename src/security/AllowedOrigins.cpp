// config/AllowedOrigins.cpp
#include "AllowedOrigins.hpp"
#include <cstdlib>
#include <sstream>

static std::unordered_set<std::string> allowedOrigins;

namespace config {

void loadAllowedOrigins() {
    const char* env = std::getenv("ALLOWED_ORIGINS");
    if (!env) return;

    std::stringstream ss(env);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);

        if (item.find("http://") == 0)
            item = item.substr(7);
        else if (item.find("https://") == 0)
            item = item.substr(8);

        allowedOrigins.insert(item);
    }
}

bool isAllowedOrigin(const std::string &originUrl) {
    std::string cleaned = originUrl;
    if (cleaned.find("http://") == 0)
        cleaned = cleaned.substr(7);
    else if (cleaned.find("https://") == 0)
        cleaned = cleaned.substr(8);

    size_t slash = cleaned.find('/');
    if (slash != std::string::npos)
        cleaned = cleaned.substr(0, slash);

    return allowedOrigins.find(cleaned) != allowedOrigins.end();
}
}

