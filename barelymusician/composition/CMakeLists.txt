add_library(
  barelymusician_duration
  duration.cpp
  duration.h
)

add_library(
  barelymusician_midi
  midi.cpp
  midi.h
)
target_link_libraries(
  barelymusician_midi
  barelymusician_pitch
)

add_library(
  barelymusician_pitch
  pitch.cpp
  pitch.h
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

  add_executable(
    barelymusician_midi_test
    midi_test.cpp
  )
  target_link_libraries(
    barelymusician_midi_test
    barelymusician_midi
    barelymusician_pitch
    gtest_main
  )
  gtest_discover_tests(barelymusician_midi_test)

  add_executable(
    barelymusician_pitch_test
    pitch_test.cpp
  )
  target_link_libraries(
    barelymusician_pitch_test
    barelymusician_pitch
    gtest_main
  )
  gtest_discover_tests(barelymusician_pitch_test)
endif()
