#pragma once
#include <string>
#include <unordered_set>

namespace config {
    void loadAllowedOrigins();
    bool isAllowedOrigin(const std::string &origin);
}

