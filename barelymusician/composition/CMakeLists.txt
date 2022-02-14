add_library(
  note_duration
  note_duration.cpp
)

add_library(
  note_pitch
  note_pitch.cpp
)

add_executable(
  note_duration_test
  note_duration_test.cpp
)
target_link_libraries(
  note_duration_test
  note_duration
  gtest_main
)
gtest_discover_tests(note_duration_test)

add_executable(
  note_pitch_test
  note_pitch_test.cpp
)
target_link_libraries(
  note_pitch_test
  note_pitch
  gtest_main
)
gtest_discover_tests(note_pitch_test)
