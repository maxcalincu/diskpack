cmake_minimum_required(VERSION 3.16)
add_library(packages SHARED tools.cpp geometry.cpp cw.pb.cc)
target_include_directories( packages
                            INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                            )