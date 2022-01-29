add_library(
  dsp_utils
  dsp_utils.cpp
)

add_library(
  envelope
  envelope.cpp
)
target_link_libraries(
  envelope
  generator
)

add_library(
  filter INTERFACE
)

add_library(
  generator INTERFACE
)

add_library(
  one_pole_filter
  one_pole_filter.cpp
)
target_link_libraries(
  one_pole_filter
  filter
)

add_library(
  oscillator
  oscillator.cpp
)
target_link_libraries(
  oscillator
  dsp_utils
  generator
)

add_library(
  polyphonic_voice INTERFACE
)

add_library(
  sample_player
  sample_player.cpp
)
target_link_libraries(
  sample_player
  generator
)

add_library(
  voice INTERFACE
)

add_executable(
  dsp_utils_test
  dsp_utils_test.cpp
)
target_link_libraries(
  dsp_utils_test
  dsp_utils
  gtest_main
)
gtest_discover_tests(dsp_utils_test)

add_executable(
  envelope_test
  envelope_test.cpp
)
target_link_libraries(
  envelope_test
  envelope
  gtest_main
)
gtest_discover_tests(envelope_test)

add_executable(
  one_pole_filter_test
  one_pole_filter_test.cpp
)
target_link_libraries(
  one_pole_filter_test
  one_pole_filter
  gtest_main
)
gtest_discover_tests(one_pole_filter_test)

add_executable(
  polyphonic_voice_test
  polyphonic_voice_test.cpp
)
target_link_libraries(
  polyphonic_voice_test
  polyphonic_voice
  gtest_main
)
gtest_discover_tests(polyphonic_voice_test)

add_executable(
  sample_player_test
  sample_player_test.cpp
)
target_link_libraries(
  sample_player_test
  sample_player
  gtest_main
)
gtest_discover_tests(sample_player_test)

