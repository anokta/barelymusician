add_library(
  note INTERFACE
)
target_link_libraries(
  note INTERFACE
  note_duration
  note_intensity
  note_pitch
)

add_library(
  note_duration
  note_duration.cpp
)

add_library(
  note_intensity INTERFACE
)

add_library(
  note_pitch
  note_pitch.cpp
)

add_library(
  sequence
  sequence.cpp
)
target_link_libraries(
  sequence
  barelymusician
  find_or_null
  id
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
