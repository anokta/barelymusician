add_library(
  barelymusician_composition_duration
  duration.cpp
  duration.h
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_composition_duration_test
    duration_test.cpp
  )
  target_link_libraries(
    barelymusician_composition_duration_test
    barelymusician_composition_duration
    gtest_main
  )
  gtest_discover_tests(barelymusician_composition_duration_test)
endif()
