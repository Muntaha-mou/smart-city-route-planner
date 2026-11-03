# Smart City Route & Infrastructure Planner
# Copyright © 2026 Quad Core. All rights reserved.

cmake_minimum_required(VERSION 3.21)

project(SmartCityPlanner
    VERSION 0.1.0
    DESCRIPTION "Smart City Route & Infrastructure Planner"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release by default so that performance measurements are meaningful.
# Use -DCMAKE_BUILD_TYPE=Debug when debugging.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

option(SCP_BUILD_APP "Build the Qt desktop application" ON)
option(SCP_BUILD_TESTS "Build the test executables" ON)

# ---------------------------------------------------------------------------
# scp_core: models, graph, CSV handling and algorithms.
# Plain C++17 + STL only (no Qt), so it can be tested without the GUI.
# ---------------------------------------------------------------------------
add_library(scp_core STATIC
    src/core/project_info.cpp
)
target_include_directories(scp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(scp_core PRIVATE SCP_VERSION="${PROJECT_VERSION}")

# ---------------------------------------------------------------------------
# Tests (CTest)
# ---------------------------------------------------------------------------
if(SCP_BUILD_TESTS)
    enable_testing()

    add_executable(graph_tests tests/graph_tests.cpp)
    target_link_libraries(graph_tests PRIVATE scp_core)
    if(MINGW)
        # Link the C++ runtime statically so tests run without extra DLLs.
        target_link_options(graph_tests PRIVATE -static)
    endif()
    add_test(NAME graph_tests COMMAND graph_tests)
endif()

# ---------------------------------------------------------------------------
# Qt Widgets desktop application
# ---------------------------------------------------------------------------
if(SCP_BUILD_APP)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
    qt_standard_project_setup()
    set(CMAKE_AUTORCC ON)

    qt_add_executable(SmartCityPlanner WIN32
        src/main.cpp
        include/ui/main_window.h
        src/ui/main_window.cpp
        resources/resources.qrc
    )
    target_link_libraries(SmartCityPlanner PRIVATE scp_core Qt6::Widgets)

    # Copy the CSV data folder next to the executable.
    add_custom_command(TARGET SmartCityPlanner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/data
                $<TARGET_FILE_DIR:SmartCityPlanner>/data
        COMMENT "Copying data folder")

    # Copy the required Qt and compiler runtime DLLs next to the executable,
    # so the application can be started by double-clicking it.
    option(SCP_DEPLOY_QT "Run windeployqt after building" ON)
    if(WIN32 AND SCP_DEPLOY_QT AND TARGET Qt6::windeployqt)
        add_custom_command(TARGET SmartCityPlanner POST_BUILD
            COMMAND Qt6::windeployqt
                    --no-translations
                    --no-system-d3d-compiler
                    --no-system-dxc-compiler
                    --no-opengl-sw
                    --verbose 0
                    $<TARGET_FILE:SmartCityPlanner>
            COMMENT "Deploying Qt runtime with windeployqt")
    endif()
endif()
