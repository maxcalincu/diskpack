cmake_minimum_required(VERSION 3.16)
project(cw)

find_package(Boost REQUIRED)

# Main library
add_library(diskpack SHARED
  src/search.cpp
  src/corona.cpp
  src/generator.cpp
  src/geometry.cpp
  src/codec.cpp
)

target_link_libraries(diskpack PRIVATE 
#  libprotobuf
  Boost::boost
)

target_include_directories(diskpack PUBLIC
# ${PROTOBUF_INSTALL_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}
)