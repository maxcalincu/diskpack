cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)

set(PROTOBUF_VERSION 3.12.4)
set(PROTOBUF_INSTALL_DIR ${CMAKE_BINARY_DIR}/_deps/protobuf-install)

ExternalProject_Add(
  protobuf_ext
  URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}/_deps/protobuf
  CONFIGURE_COMMAND 
    <SOURCE_DIR>/configure 
    --prefix=${PROTOBUF_INSTALL_DIR}
    --enable-shared
    --disable-static
  BUILD_COMMAND $(MAKE)
  INSTALL_COMMAND $(MAKE) install
)

add_library(libprotobuf SHARED IMPORTED)
add_dependencies(libprotobuf protobuf_ext)

file(MAKE_DIRECTORY ${PROTOBUF_INSTALL_DIR}/include)  # Create dirs upfront
set_target_properties(libprotobuf PROPERTIES
  IMPORTED_LOCATION ${PROTOBUF_INSTALL_DIR}/lib/libprotobuf.so
  INTERFACE_INCLUDE_DIRECTORIES ${PROTOBUF_INSTALL_DIR}/include
)

add_library(packages SHARED
  tools.cpp
  geometry.cpp
  cw.pb.cc
  generator.cpp
  corona.cpp
)

target_link_libraries(packages PRIVATE libprotobuf)
target_include_directories(packages PUBLIC
  ${PROTOBUF_INSTALL_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)