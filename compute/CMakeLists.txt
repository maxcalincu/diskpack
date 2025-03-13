cmake_minimum_required(VERSION 3.16)

project(cw)

include(FetchContent)

set(PROTOBUF_VERSION 3.12.4)
set(PROTOBUF_URL https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-cpp-${PROTOBUF_VERSION}.tar.gz)

FetchContent_Declare(
  protobuf
  URL ${PROTOBUF_URL}
)

FetchContent_MakeAvailable(protobuf)

add_subdirectory(packages)
add_executable(main main.cpp)

target_link_libraries(main packages)
target_link_libraries(main protobuf)
target_link_libraries(main pthread)