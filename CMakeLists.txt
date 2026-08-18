cmake_minimum_required(VERSION 3.11)

project(sift
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SIFT_BUILD_TESTS "Build unit tests" ON)
option(SIFT_BUILD_BENCHMARKS "Build benchmarks" OFF)

# core library

add_library(sift_core INTERFACE)
add_library(sift::core ALIAS sift_core)

target_include_directories(sift_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(sift_core INTERFACE cxx_std_20)

# Catch2 is shared by tests and benchmarks
if(SIFT_BUILD_TESTS OR SIFT_BUILD_BENCHMARKS)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.15.3
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# tests
if(SIFT_BUILD_TESTS)
    add_executable(gh_filter_test tests/gh_filter_test.cpp)
    target_link_libraries(
        gh_filter_test
        PRIVATE
            sift::core
            Catch2::Catch2WithMain
    )

    include(CTest)
    include(Catch)
    catch_discover_tests(gh_filter_test)
endif()

# benchmarks
if(SIFT_BUILD_BENCHMARKS)
    add_executable(
        gh_filter_benchmark
        benchmarks/gh_filter_benchmark.cpp
    )
    target_link_libraries(
        gh_filter_benchmark
        PRIVATE
            sift::core
            Catch2::Catch2WithMain
    )
endif()
