cmake_minimum_required(VERSION 3.16)
project(cw)
find_package(Threads REQUIRED)             

# add_subdirectory(../disk-packing-generator ${CMAKE_BINARY_DIR}/diskpack)

add_executable(visualizer src/main.cpp)
target_link_libraries(visualizer PRIVATE
    diskpack 
    Threads::Threads
)