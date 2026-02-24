cmake_minimum_required(VERSION 3.18)
project(jumsparse LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# ── Build options ────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build the test suite" OFF)

# ── Library ──────────────────────────────────────────────────────────────────
add_subdirectory(src)

# ── Tests ────────────────────────────────────────────────────────────────────
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
