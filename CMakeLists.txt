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
        resources.qrc
        file_info.rc
        managements/AccountManager.h
        utils/DataStructures.h
        managements/RatingManager.h
        dialogs/AccountLoginDialog.cpp
        dialogs/AccountLoginDialog.h
        utils/Sha256Util.h
        managements/AccountManager.cpp
        dialogs/oobeDialog.cpp
        dialogs/oobeDialog.h
        dialogs/StyledDialog.h
        dialogs/StyledDialog.cpp
        utils/AsyncJsonIO.h
        MainWindow.cpp
        MainWindow.h
        pages/StudentPage.cpp
        pages/StudentPage.h
        managements/ClassManager.cpp
        managements/ClassManager.h
        pages/components/StatCard.cpp
        pages/components/StatCard.h
        pages/AdminDashboardPage.cpp
        pages/AdminDashboardPage.h
        pages/components/DistributionRing.cpp
        pages/components/DistributionRing.h
        pages/model/StudentTableModel.h
        pages/TeacherPage.cpp
        pages/TeacherPage.h
        pages/model/TeacherTableModel.h
        dialogs/AddTeacherDialog.cpp
        dialogs/AddTeacherDialog.h
        pages/delegate/OperationDelegate.cpp
        pages/delegate/OperationDelegate.h
        dialogs/TeacherDetailDialog.cpp
        dialogs/TeacherDetailDialog.h
        pages/model/FilterProxyModel.h
        dialogs/AddDepartmentDialog.cpp
        dialogs/AddDepartmentDialog.h
        pages/ClassPage.cpp
        pages/ClassPage.h
        pages/model/ClassTableModel.h
        dialogs/ClassDetailDialog.cpp
        dialogs/ClassDetailDialog.h
        dialogs/AddClassDialog.cpp
        dialogs/AddClassDialog.h
        dialogs/AddStudentDialog.cpp
        dialogs/AddStudentDialog.h
        managements/CourseManager.cpp
        managements/CourseManager.h
        dialogs/StudentDetailDialog.cpp
        dialogs/StudentDetailDialog.h
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
    if (MSVC)
        set(MANIFEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml")
        add_custom_command(TARGET AAIMS POST_BUILD
                COMMAND mt.exe -manifest ${MANIFEST_FILE} -outputresource:$<TARGET_FILE:AAIMS>;#1
                COMMENT "Embedding UAC manifest..."
        )
    elseif (MINGW)
        set(MANIFEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/manifest.xml")
        set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/resources.rc")

        file(WRITE ${RC_FILE}
                "1 24 \"${MANIFEST_FILE}\"\n"
        )

        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.o
                COMMAND windres -i ${RC_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/resources.o
                DEPENDS ${RC_FILE} ${MANIFEST_FILE}
        )

        target_sources(AAIMS PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/resources.o)
    endif ()
endif ()
