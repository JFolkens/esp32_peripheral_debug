cmake_minimum_required(VERSION 3.5)

# Populate sdkconfig in build folder instead of root level
set(SDKCONFIG "${CMAKE_BINARY_DIR}/sdkconfig")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(rover)
