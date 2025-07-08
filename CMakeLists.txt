cmake_minimum_required(VERSION 3.16)
project(drogon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

add_executable(drogon
    src/main.cpp
    src/routes/RootHandler.cpp
    src/db/initialize.cpp
    src/db/AccessLog.cpp
    src/security/SecurityMiddleware.cpp
    src/security/ApiKeyValidator.cpp
    src/security/IpControl.cpp
    src/config/AllowedOrigins.cpp
)

target_link_libraries(drogon 
  PRIVATE Drogon::Drogon
  PRIVATE sqlite3
)

target_include_directories(drogon
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
