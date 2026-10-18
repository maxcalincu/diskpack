include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Catch2)

add_library(catch_main "${PROJECT_SOURCE_DIR}/tools/catch_main.cpp")
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
