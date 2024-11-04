set(CMAKE_FOLDER src)

include_directories(.)

add_library(
  barelycomposer_headeronly INTERFACE
  ${CMAKE_SOURCE_DIR}/include/barelycomposer.h
)
target_link_libraries(
  barelycomposer_headeronly INTERFACE
  barelymusician_headeronly
)

add_library(
  barelycomposer STATIC
  barelycomposer.cpp
)
set_target_properties(
  barelycomposer PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_compile_definitions(
  barelycomposer
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelycomposer
  barelymusician
  barelymusician_common_random
  barelymusician_components_arpeggiator
  barelymusician_components_repeater
  barelymusician_internal_musician
)

add_library(
  barelymusician_headeronly INTERFACE
  ${CMAKE_SOURCE_DIR}/include/barelymusician.h
)

add_library(
  barelymusician STATIC
  barelymusician.cpp
)
set_target_properties(
  barelymusician PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_compile_definitions(
  barelymusician
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician
  barelymusician_headeronly
  barelymusician_internal_instrument
  barelymusician_internal_musician
  barelymusician_internal_performer
)

if(ENABLE_BENCHMARKS)
  set(CMAKE_FOLDER external)

  FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG        v1.9.0
  )
  set(BENCHMARK_ENABLE_TESTING OFF)
  FetchContent_MakeAvailable(benchmark)
  target_compile_options(
    benchmark PUBLIC
    -w
  )
  include_directories(${benchmark_SOURCE_DIR}/include)

  set(CMAKE_FOLDER src)

  add_executable(
    barelymusician_benchmark
    barelymusician_benchmark.cpp
  )
  target_link_libraries(
    barelymusician_benchmark
    barelymusician
    benchmark_main
  )
endif()

if(ENABLE_TESTS)
  enable_testing()

  set(CMAKE_FOLDER external)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
  )
  if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()
  FetchContent_MakeAvailable(googletest)
  include(GoogleTest)

  set(CMAKE_FOLDER src)

  add_executable(
    barelycomposer_test
    barelycomposer_test.cpp
  )
  target_link_libraries(
    barelycomposer_test
    barelycomposer
    gtest_main
  )
  gtest_discover_tests(barelycomposer_test)

  add_executable(
    barelymusician_test
    barelymusician_test.cpp
  )
  target_link_libraries(
    barelymusician_test
    barelymusician
    gtest_main
  )
  gtest_discover_tests(barelymusician_test)
endif()

add_subdirectory(algorithm)
add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(dsp)
add_subdirectory(internal)
