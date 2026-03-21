# This file is part of AAIMS.
# You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
# See https://spdx.org/licenses/BSD-3-Clause.html

cmake_minimum_required(VERSION 4.1)
project(AcademicAffairsInformationManagementSystemServer CXX)

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

add_executable(AcademicAffairsInformationManagementSystemServer main.cpp
        file_manager.h
        account_manager.h
        data_structures.h
        config_manager.h
        lesson_manager.h
        rating_manager.h
)

target_link_libraries(AcademicAffairsInformationManagementSystemServer
        PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
)

if(NOT MSVC)
    target_compile_options(AcademicAffairsInformationManagementSystemServer PRIVATE -Wall -Wextra -O3)
endif()