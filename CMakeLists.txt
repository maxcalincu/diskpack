cmake_minimum_required(VERSION 3.16)
project(cw)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(diskpack)
add_subdirectory(visualization-tool)
add_subdirectory(configuration-finder)