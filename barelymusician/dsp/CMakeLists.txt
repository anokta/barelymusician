add_library(
  dsp_utils
  dsp_utils.cpp
  dsp_utils.h
)
target_link_libraries(
  dsp_utils
  barelymusician
)

add_library(
  envelope
  envelope.cpp
  envelope.h
)
target_link_libraries(
  envelope
  generator
  barelymusician
)

add_library(
  filter INTERFACE
  filter.h
)
target_link_libraries(
  filter
  barelymusician
)

add_library(
  gain_processor
  gain_processor.cpp
  gain_processor.h
)
target_link_libraries(
  gain_processor
  barelymusician
)

add_library(
  generator INTERFACE
  generator.h
)
target_link_libraries(
  generator
  barelymusician
)

add_library(
  one_pole_filter
  one_pole_filter.cpp
  one_pole_filter.h
)
target_link_libraries(
  one_pole_filter
  filter
  barelymusician
)

add_library(
  oscillator
  oscillator.cpp
  oscillator.h
)
target_link_libraries(
  oscillator
  dsp_utils
  generator
  barelymusician
  random
)

add_library(
  polyphonic_voice INTERFACE
  polyphonic_voice.h
)
target_link_libraries(
  polyphonic_voice
  barelymusician
)

add_library(
  sample_player
  sample_player.cpp
  sample_player.h
)
target_link_libraries(
  sample_player
  generator
  barelymusician
)

add_library(
  voice INTERFACE
  voice.h
)
target_link_libraries(
  voice
  barelymusician
)

add_executable(
  dsp_utils_test
  dsp_utils_test.cpp
)
target_link_libraries(
  dsp_utils_test
  dsp_utils
  barelymusician
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
  barelymusician
  gtest_main
)
gtest_discover_tests(envelope_test)

add_executable(
  gain_processor_test
  gain_processor_test.cpp
)
target_link_libraries(
  gain_processor_test
  gain_processor
  barelymusician
  gtest_main
)
gtest_discover_tests(gain_processor_test)

add_executable(
  one_pole_filter_test
  one_pole_filter_test.cpp
)
target_link_libraries(
  one_pole_filter_test
  one_pole_filter
  barelymusician
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
  barelymusician
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
  barelymusician
  gtest_main
)
gtest_discover_tests(sample_player_test)

