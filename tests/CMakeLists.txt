#[[
CMake instructions for unit tests ONLY.

This folder should not be linked by the embedded project. It is for
compilation on local host machine, not on the embedded platform.
]]

cmake_minimum_required(VERSION 3.14)
project(unit_tests CXX)
set(project "unit_tests")
enable_testing()

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest is expected to be installed once into tests/.deps/gtest-install
# by the batch script, and then reused by CMake on later runs.
find_package(GTest CONFIG REQUIRED)

# Automatically collect test sources. CONFIGURE_DEPENDS ensures fresh runs
# re-scan on changes.
file(GLOB_RECURSE _test_sources
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.cc
    *.cxx
)

# Exclude dependency trees such as tests/.deps so GoogleTest is not compiled
# into the unit-test executable from source.
list(FILTER _test_sources EXCLUDE REGEX "(^|/)(\\.deps|\\.build)(/|$)")

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)

file(GLOB_RECURSE _main_sources
    CONFIGURE_DEPENDS
    ${REPO_ROOT}/main/**/*.cpp
)

# Exclude specific hardware implementations that are not meant to be compiled
# for a host architecture
list(FILTER _main_sources EXCLUDE REGEX "(^|/)(\\esp32)(/|$)")

add_executable(${project}
    ${_test_sources}
    ${_main_sources}
)

target_link_libraries(
    ${project}
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
)

target_include_directories(${project}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${REPO_ROOT}
)

include(GoogleTest)
gtest_discover_tests(${project})
