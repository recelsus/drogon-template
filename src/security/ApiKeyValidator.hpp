// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#pragma once
#include <string>
#include <optional>

namespace api_validator {
    std::optional<std::string> getApiKeyLevel(const std::string &key);
}

