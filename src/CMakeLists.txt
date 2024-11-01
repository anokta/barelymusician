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
  barelymusician_internal_instrument_controller
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
  barelymusician_internal_instrument_controller
  barelymusician_internal_musician
  barelymusician_internal_performer
)

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
add_subdirectory(components)
add_subdirectory(dsp)
add_subdirectory(internal)
