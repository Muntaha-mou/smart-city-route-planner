# Smart City Route & Infrastructure Planner
# Copyright © 2026 Quad Core. All rights reserved.

cmake_minimum_required(VERSION 3.21)
project(SmartCityPlanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Show helpful compiler warnings
if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
endif()

# The application
add_executable(SmartCityPlanner
    src/main.cpp
    src/graph.cpp
)
target_include_directories(SmartCityPlanner PRIVATE include)

# The tests (run with: ctest --test-dir build)
add_executable(tests
    tests/tests.cpp
    src/graph.cpp
)
target_include_directories(tests PRIVATE include)

enable_testing()
add_test(NAME tests COMMAND tests)
