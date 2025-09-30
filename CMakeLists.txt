cmake_minimum_required(VERSION 4.0)
project(webhook)

set(CMAKE_CXX_STANDARD 23)
set(USE_TLS ON)

add_executable(webhook
        include/webhook/server.h
        src/server.cpp
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

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(webhook PRIVATE nlohmann_json::nlohmann_json)

FetchContent_Declare(ixwebsocket
    GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    GIT_TAG v11.4.6
)
FetchContent_MakeAvailable(ixwebsocket)
target_link_libraries(webhook PRIVATE ixwebsocket)

target_compile_definitions(webhook PUBLIC -DJSON_DIAGNOSTICS=1)