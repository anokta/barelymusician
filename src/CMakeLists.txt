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
  barelymusician_private_arpeggiator_impl
  barelymusician_private_engine_impl
  barelymusician_private_random_impl
  barelymusician_private_repeater_impl
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
  barelymusician_private_engine_impl
  barelymusician_private_instrument_impl
  barelymusician_private_performer_impl
)

if(ENABLE_BENCHMARKS)
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
add_subdirectory(dsp)
add_subdirectory(private)
