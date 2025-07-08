// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#pragma once
#include <string>

namespace ip_control {
bool isWhitelisted(const std::string &ip);
bool isBlacklisted(const std::string &ip);
}
