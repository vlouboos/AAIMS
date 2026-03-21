# This file is part of AAIMS.
# You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
# See https://spdx.org/licenses/BSD-3-Clause.html

cmake_minimum_required(VERSION 4.1)
project(AcademicAffairsInformationManagementSystemServer C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_Declare(
        uWebSockets
        GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
        GIT_TAG v20.75.0
)

FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.2
)

FetchContent_Declare(
        libuv
        GIT_REPOSITORY https://github.com/libuv/libuv.git
        GIT_TAG v1.52.1
)

FetchContent_MakeAvailable(json uWebSockets zlib libuv)

file(GLOB US_SOURCES
        ${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/uSockets/src/*.c
        ${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/uSockets/src/eventing/*.c
)

add_executable(AcademicAffairsInformationManagementSystemServer main.cpp
        file_manager.h
        account_manager.h
        data_structures.h
        config_manager.h
        lesson_manager.h
        rating_manager.h
        ${US_SOURCES}
)

target_include_directories(AcademicAffairsInformationManagementSystemServer
        PRIVATE
        ${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/src
        ${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/uSockets/src
)

target_link_libraries(AcademicAffairsInformationManagementSystemServer
        PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
        ws2_32
        iphlpapi
        uv_a
        z
)

target_compile_definitions(AcademicAffairsInformationManagementSystemServer
        PRIVATE
        LIBUS_NO_SSL
)

if (NOT MSVC)
    target_compile_options(AcademicAffairsInformationManagementSystemServer PRIVATE -Wall -Wextra -O3)
    target_link_options(AcademicAffairsInformationManagementSystemServer PRIVATE
            -static-libgcc
            -static-libstdc++
            -static
    )
endif ()