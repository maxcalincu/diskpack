cmake_minimum_required(VERSION 3.16)
project(cw)

set(DEPS_DIR ${CMAKE_SOURCE_DIR}/_deps)
set(PROTOBUF_VERSION 3.12.4)

set(PROTOBUF_INSTALL_DIR ${DEPS_DIR}/protobuf-install)
set(PROTOBUF_PREFIX ${DEPS_DIR}/protobuf)
set(PROTOBUF_DOWNLOAD_DIR ${DEPS_DIR}/downloads)

include(ExternalProject)
file(MAKE_DIRECTORY ${DEPS_DIR})
file(MAKE_DIRECTORY ${PROTOBUF_DOWNLOAD_DIR})

ExternalProject_Add(
  protobuf_ext
  URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz
  DOWNLOAD_DIR ${PROTOBUF_DOWNLOAD_DIR}
  PREFIX ${PROTOBUF_PREFIX}
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
file(MAKE_DIRECTORY ${PROTOBUF_INSTALL_DIR}/include)
set_target_properties(libprotobuf PROPERTIES
  IMPORTED_LOCATION ${PROTOBUF_INSTALL_DIR}/lib/libprotobuf.so
  INTERFACE_INCLUDE_DIRECTORIES ${PROTOBUF_INSTALL_DIR}/include
)

find_package(Boost REQUIRED)

# Main library
add_library(diskpack SHARED
  generated/cw.pb.cc
  src/corona.cpp
  src/generator.cpp
  src/geometry.cpp
  src/checkers.cpp
)

target_link_libraries(diskpack PRIVATE 
  libprotobuf
  Boost::boost
)

target_include_directories(diskpack PUBLIC
  ${PROTOBUF_INSTALL_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}
)