cmake_minimum_required(VERSION 3.16)
project(service_honeypot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(CURL REQUIRED)

add_executable(service_honeypot
        src/main.cpp
        src/server.cpp
        src/session.cpp
        src/fake_shell.cpp
        src/utils.cpp
        src/filesystem.cpp
        include/logger.hpp
        src/logger.cpp
        src/discord_notifier.cpp
        src/discord_payload.cpp
        src/discord_global.cpp
)

target_include_directories(service_honeypot PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(service_honeypot PRIVATE CURL::libcurl)
