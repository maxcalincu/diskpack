cmake_minimum_required(VERSION 3.16)

project(cw)
# find_package(libprotobuf REQUIRED)

add_subdirectory(packages)
add_executable(main main.cpp)

target_link_libraries(main packages)
target_link_libraries(main protobuf)
target_link_libraries(main pthread)
# g++ packages/tools.cpp packages/geometry.cpp packages/cw.pb.cc main.cpp `pkg-config --cflags --libs protobuf` -o main && ./main ../storage/1.txt