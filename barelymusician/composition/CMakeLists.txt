add_library(
  barely_intensity
  intensity.cpp
  intensity.h
)

add_library(
  barely_pitch
  pitch.cpp
  pitch.h
)
target_link_libraries(
  barely_pitch
  barely_rational
  barelymusician_headeronly
)

if (ENABLE_TESTS)
  add_executable(
    barely_intensity_test
    intensity_test.cpp
  )
  target_link_libraries(
    barely_intensity_test
    barely_intensity
    gtest_main
  )
  gtest_discover_tests(barely_intensity_test)

  add_executable(
    barely_pitch_test
    pitch_test.cpp
  )
  target_link_libraries(
    barely_pitch_test
    barely_pitch
    barely_rational
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barely_pitch_test)
endif()
