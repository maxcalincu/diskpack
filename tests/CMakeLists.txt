find_package(Boost REQUIRED)
include(Catch)

set(DISKPACK_TEST_MODULES regions geometry corona graph generator search codec cli)
set(DISKPACK_TEST_SOURCES)
foreach(MODULE IN LISTS DISKPACK_TEST_MODULES)
  list(APPEND DISKPACK_TEST_SOURCES "${MODULE}.cpp")
  add_catch(test_${MODULE} "${MODULE}.cpp")
  target_link_libraries(test_${MODULE} PRIVATE diskpack Boost::boost)
  if(MODULE STREQUAL "cli")
    target_compile_definitions(test_${MODULE} PRIVATE
      DISKPACK_FINDER_PATH="$<TARGET_FILE:finder>"
      DISKPACK_VISUALIZER_PATH="$<TARGET_FILE:visualizer>")
    add_dependencies(test_${MODULE} finder visualizer)
  endif()
endforeach()

add_catch(test_diskpack ${DISKPACK_TEST_SOURCES})
target_link_libraries(test_diskpack PRIVATE diskpack Boost::boost)
target_compile_definitions(test_diskpack PRIVATE
  DISKPACK_FINDER_PATH="$<TARGET_FILE:finder>"
  DISKPACK_VISUALIZER_PATH="$<TARGET_FILE:visualizer>")
add_dependencies(test_diskpack finder visualizer)
catch_discover_tests(test_diskpack
  TEST_PREFIX "diskpack."
  PROPERTIES TIMEOUT 60
)

option(DISKPACK_ENABLE_COVERAGE "Instrument project code and tests for coverage" OFF)
if(DISKPACK_ENABLE_COVERAGE)
  set(COVERAGE_TARGETS diskpack finder visualizer test_diskpack)
  foreach(MODULE IN LISTS DISKPACK_TEST_MODULES)
    list(APPEND COVERAGE_TARGETS test_${MODULE})
  endforeach()
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/coverage")
  foreach(TARGET IN LISTS COVERAGE_TARGETS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      target_compile_options(${TARGET} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
      target_link_options(${TARGET} PRIVATE -fprofile-instr-generate)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${TARGET} PRIVATE --coverage)
      target_link_options(${TARGET} PRIVATE --coverage)
    else()
      message(FATAL_ERROR "Coverage requires GCC or Clang")
    endif()
  endforeach()
endif()
