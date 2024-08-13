add_library(
  barely_duration
  duration.cpp
  duration.h
)

add_library(
  barely_midi
  midi.cpp
  midi.h
)
target_link_libraries(
  barely_midi
  barely_pitch
)

add_library(
  barely_pitch
  pitch.cpp
  pitch.h
)

if(ENABLE_TESTS)
  add_executable(
    barely_duration_test
    duration_test.cpp
  )
  target_link_libraries(
    barely_duration_test
    barely_duration
    gtest_main
  )
  gtest_discover_tests(barely_duration_test)

  add_executable(
    barely_midi_test
    midi_test.cpp
  )
  target_link_libraries(
    barely_midi_test
    barely_midi
    barely_pitch
    gtest_main
  )
  gtest_discover_tests(barely_midi_test)

  add_executable(
    barely_pitch_test
    pitch_test.cpp
  )
  target_link_libraries(
    barely_pitch_test
    barely_pitch
    gtest_main
  )
  gtest_discover_tests(barely_pitch_test)
endif()
