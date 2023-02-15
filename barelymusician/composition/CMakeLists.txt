add_library(
  duration
  duration.cpp
  duration.h
)

add_library(
  pitch
  pitch.cpp
  pitch.h
)

add_executable(
  duration_test
  duration_test.cpp
)
target_link_libraries(
  duration_test
  duration
  gtest_main
)
gtest_discover_tests(duration_test)

add_executable(
  pitch_test
  pitch_test.cpp
)
target_link_libraries(
  pitch_test
  pitch
  gtest_main
)
gtest_discover_tests(pitch_test)
