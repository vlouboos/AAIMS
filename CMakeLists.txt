# This file is part of AAIMS.
# You WON'T be guaranteed to be permitted with this file unless you're under BSD-3 License.
# See https://spdx.org/licenses/BSD-3-Clause.html

cmake_minimum_required(VERSION 4.1)
project(AAIMS C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(json)

add_executable(AAIMS main.cpp
        FileManager.h
        AccountManager.h
        DataStructures.h
        LessonManager.h
        RatingManager.h
        AccountLoginDialog.cpp
        AccountLoginDialog.h
        resources.qrc
        sha256util.h
        AccountManager.cpp
)

target_link_libraries(AAIMS
        PRIVATE
        Threads::Threads
        nlohmann_json::nlohmann_json
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt::Svg
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

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    get_target_property(_qt_core_path Qt6::Core LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_core_path}" DIRECTORY)
    find_program(QT_DEPLOY_TOOL NAMES windeployqt windeployqt.exe HINTS "${_qt_bin_dir}")

    # 2. 执行自动化部署
    if (QT_DEPLOY_TOOL)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${QT_DEPLOY_TOOL} --no-translations --compiler-runtime "$<TARGET_FILE:${PROJECT_NAME}>"
                COMMENT "Running windeployqt to copy all dependencies..."
        )
    else()
        message(WARNING "windeployqt not found, deployment skipped!")
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${QT_DEPLOY_TOOL} --no-translations --compiler-runtime "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt dependencies for distribution..."
)
