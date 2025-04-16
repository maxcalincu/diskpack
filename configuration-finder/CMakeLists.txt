cmake_minimum_required(VERSION 3.16)
project(cw)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(finder 
    src/main.cpp
)

target_link_libraries(finder PRIVATE
    diskpack 
    Boost::program_options
)