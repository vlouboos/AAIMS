# This file is part of AAIMS.
# You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
# See https://spdx.org/licenses/BSD-3-Clause.html

cmake_minimum_required(VERSION 4.1)
project(AAIMS C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(json)

add_executable(AAIMS main.cpp
        ${US_SOURCES}
        file_manager.h
        account_manager.h
        data_structures.h
        lesson_manager.h
        rating_manager.h
        connection.h
)

target_link_libraries(AAIMS
        PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
        ${SYSTEM_LIBS}
)

target_compile_definitions(AAIMS
        PRIVATE
        LIBUS_NO_SSL
)

if (NOT MSVC)
    target_compile_options(AAIMS PRIVATE -Wall -Wextra -O3)
    if (WIN32)
        target_link_options(AAIMS PRIVATE
                -static-libgcc
                -static-libstdc++
                -static
        )
    endif ()
endif ()