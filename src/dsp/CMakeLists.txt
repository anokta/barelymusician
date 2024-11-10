add_library(
  barelymusician_dsp_decibels
  decibels.cpp
  decibels.h
)

add_library(
  barelymusician_dsp_envelope
  envelope.cpp
  envelope.h
)

add_library(
  barelymusician_dsp_gain_processor
  gain_processor.cpp
  gain_processor.h
)

add_library(
  barelymusician_dsp_one_pole_filter INTERFACE
  one_pole_filter.h
)
target_link_libraries(
  barelymusician_dsp_one_pole_filter INTERFACE
  barelymusician_headeronly
)

add_library(
  barelymusician_dsp_oscillator INTERFACE
  oscillator.h
)
target_link_libraries(
  barelymusician_dsp_oscillator INTERFACE
  barelymusician_common_random
  barelymusician_headeronly
)

add_library(
  barelymusician_dsp_sample_player
  sample_player.cpp
  sample_player.h
)

add_library(
  barelymusician_dsp_voice
  voice.cpp
  voice.h
)
target_link_libraries(
  barelymusician_dsp_voice
  barelymusician_dsp_envelope
  barelymusician_dsp_one_pole_filter
  barelymusician_dsp_oscillator
  barelymusician_dsp_sample_player
  barelymusician_headeronly
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_dsp_decibels_test
    decibels_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_decibels_test
    barelymusician_dsp_decibels
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_decibels_test)

  add_executable(
    barelymusician_dsp_envelope_test
    envelope_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_envelope_test
    barelymusician_dsp_envelope
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_envelope_test)

  add_executable(
    barelymusician_dsp_gain_processor_test
    gain_processor_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_gain_processor_test
    barelymusician_dsp_gain_processor
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_gain_processor_test)

  add_executable(
    barelymusician_dsp_one_pole_filter_test
    one_pole_filter_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_one_pole_filter_test
    barelymusician_dsp_one_pole_filter
    barelymusician
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_one_pole_filter_test)

  add_executable(
    barelymusician_dsp_sample_player_test
    sample_player_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_sample_player_test
    barelymusician_dsp_sample_player
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_sample_player_test)
endif()
