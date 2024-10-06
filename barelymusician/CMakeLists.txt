set(CMAKE_FOLDER barelymusician)

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
  barelymusician_internal_observable
  barelymusician_internal_performer
  barelymusician_internal_task
)

add_library(
  barelymusician_headeronly INTERFACE
  barelymusician.h
)

if(ENABLE_TESTS)
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
add_subdirectory(composition)
add_subdirectory(dsp)
add_subdirectory(instruments)
add_subdirectory(internal)
