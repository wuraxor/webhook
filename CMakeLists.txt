cmake_minimum_required(VERSION 4.0)
project(webhook)

set(CMAKE_CXX_STANDARD 23)

add_executable(webhook
        include/webhook/server.h
        include/webhook/controller.h
        src/server.cpp
        src/controller.cpp
        src/main.cpp
        src/discord.cpp
        include/webhook/discord.h
        src/events.cpp
        include/webhook/events.h
        src/github.cpp
        include/webhook/github.h
)

target_include_directories(webhook PUBLIC include/)

include(FetchContent)

FetchContent_Declare(drogon
    GIT_REPOSITORY https://github.com/drogonframework/drogon.git
    GIT_TAG v1.9.11
)
FetchContent_MakeAvailable(drogon)
target_link_libraries(webhook PRIVATE drogon)