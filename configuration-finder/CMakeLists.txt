cmake_minimum_required(VERSION 3.16)
project(cw)

find_package(Threads REQUIRED)

add_executable(finder 
    src/main.cpp
)

target_include_directories(finder PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(finder PRIVATE
    diskpack 
    Threads::Threads
)