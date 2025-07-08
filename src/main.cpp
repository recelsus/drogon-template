// vim: filetype=cpp:expandtab:ts=4:sw=4:softtabstop=4
#include <drogon/drogon.h>
#include "db/initialize.hpp"

void registerRootHandler(); // routes/RootHandler.cpp

int main() {
    init_db::initialize();

    registerRootHandler();

    std::cout << "Starting server with Drogon" << "\n";
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .run();

    return 0;
}

