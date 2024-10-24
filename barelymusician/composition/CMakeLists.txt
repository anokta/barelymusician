add_library(
  barelymusician_duration
  duration.cpp
  duration.h
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_duration_test
    duration_test.cpp
  )
  target_link_libraries(
    barelymusician_duration_test
    barelymusician_duration
    gtest_main
  )
  gtest_discover_tests(barelymusician_duration_test)
endif()
