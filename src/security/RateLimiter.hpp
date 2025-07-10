#pragma once
#include <string>

namespace rate_limiter {
    bool isAllowed(const std::string &ip, const std::string &endpoint, int max_requests_per_window = 10, int window_seconds = 60);
}

