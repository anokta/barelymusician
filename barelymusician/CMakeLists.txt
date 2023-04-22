add_subdirectory(algorithm)
add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(composition)
add_subdirectory(dsp)
add_subdirectory(effects)
add_subdirectory(instruments)
add_subdirectory(internal)

add_library(
  barelymusician
  barelymusician.cpp
)
target_compile_definitions(
  barelymusician
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician
  barelymusician_headeronly
  engine
)

add_library(
  barelymusician_headeronly INTERFACE
  barelymusician.h
)

if (ENABLE_TESTS)
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
