cmake_minimum_required(VERSION 3.16)

include(FetchContent)

set(PROTOBUF_VERSION 3.12.4)
set(PROTOBUF_URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz)

FetchContent_Declare(
  protobuf
  URL ${PROTOBUF_URL}
)

FetchContent_MakeAvailable(protobuf)

add_library(packages SHARED tools.cpp geometry.cpp cw.pb.cc generator.cpp corona.cpp)

target_link_libraries(packages protobuf)

target_include_directories( packages
                            INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                            )
