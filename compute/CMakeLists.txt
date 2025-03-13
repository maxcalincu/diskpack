cmake_minimum_required(VERSION 3.16)

project(cw)

add_subdirectory(packages)
add_executable(main main.cpp)

target_link_libraries(main packages)
target_link_libraries(main pthread)