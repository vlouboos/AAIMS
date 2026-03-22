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
set(RELEASE_DIR "${CMAKE_SOURCE_DIR}/build/libs")

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Concurrent)

include(FetchContent)

add_executable(AAIMS main.cpp
        AccountManager.h
        DataStructures.h
        LessonManager.h
        RatingManager.h
        AccountLoginDialog.cpp
        AccountLoginDialog.h
        resources.qrc
        sha256util.h
        AccountManager.cpp
        oobeDialog.cpp
        oobeDialog.h
        StyledDialog.h
        StyledDialog.cpp
        AsyncJsonIO.h
)

target_link_libraries(AAIMS
        PRIVATE
        Threads::Threads
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
        Qt6::Concurrent
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

if (WIN32)
    # 1. 确保获取了工具路径
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(QT_DEPLOY_TOOL NAMES windeployqt.exe windeployqt HINTS "${_qt_bin_dir}")

    set(RELEASE_DIR "${CMAKE_SOURCE_DIR}/build/libs")

    add_custom_target(AlwaysDeploy ALL
            DEPENDS ${PROJECT_NAME}
            # A. 确保目录存在
            COMMAND ${CMAKE_COMMAND} -E make_directory "${RELEASE_DIR}"

            # B. 强制拷贝 (即使文件已经存在也会覆盖)
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${RELEASE_DIR}/"

            # C. 执行部署
            COMMAND "${QT_DEPLOY_TOOL}"
            --no-translations
            --compiler-runtime
            "${RELEASE_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"

            COMMENT ">>> [Shadow Copy] Deploying to build/libs..."
            VERBATIM
    )
endif ()
