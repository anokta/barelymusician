add_library(
  note_duration
  note_duration.cpp
  note_duration.h
)

add_library(
  note_pitch
  note_pitch.cpp
  note_pitch.h
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