add_library(
  barely_decibels
  decibels.cpp
  decibels.h
)

add_library(
  barely_dsp_utils
  dsp_utils.cpp
  dsp_utils.h
)

add_library(
  barely_envelope
  envelope.cpp
  envelope.h
)
target_link_libraries(
  barely_envelope
  barely_generator
)

add_library(
  barely_enveloped_voice INTERFACE
  enveloped_voice.h
)
target_link_libraries(
  barely_enveloped_voice INTERFACE
  barely_envelope
  barely_voice
)

add_library(
  barely_filter INTERFACE
  filter.h
)

add_library(
  barely_gain_processor
  gain_processor.cpp
  gain_processor.h
)

add_library(
  barely_generator INTERFACE
  generator.h
)

add_library(
  barely_one_pole_filter
  one_pole_filter.cpp
  one_pole_filter.h
)
target_link_libraries(
  barely_one_pole_filter
  barely_filter
)

add_library(
  barely_oscillator
  oscillator.cpp
  oscillator.h
)
target_link_libraries(
  barely_oscillator
  barely_dsp_utils
  barely_generator
  barely_random
)

add_library(
  barely_polyphonic_voice INTERFACE
  polyphonic_voice.h
)

add_library(
  barely_sample_player
  sample_player.cpp
  sample_player.h
)
target_link_libraries(
  barely_sample_player
  barely_generator
)

add_library(
  barely_voice INTERFACE
  voice.h
)

if(ENABLE_TESTS)
  add_executable(
    barely_decibels_test
    decibels_test.cpp
  )
  target_link_libraries(
    barely_decibels_test
    barely_decibels
    gtest_main
  )
  gtest_discover_tests(barely_decibels_test)

  add_executable(
    barely_dsp_utils_test
    dsp_utils_test.cpp
  )
  target_link_libraries(
    barely_dsp_utils_test
    barely_dsp_utils
    gtest_main
  )
  gtest_discover_tests(barely_dsp_utils_test)

  add_executable(
    barely_envelope_test
    envelope_test.cpp
  )
  target_link_libraries(
    barely_envelope_test
    barely_envelope
    gtest_main
  )
  gtest_discover_tests(barely_envelope_test)

  add_executable(
    barely_gain_processor_test
    gain_processor_test.cpp
  )
  target_link_libraries(
    barely_gain_processor_test
    barely_gain_processor
    gtest_main
  )
  gtest_discover_tests(barely_gain_processor_test)

  add_executable(
    barely_one_pole_filter_test
    one_pole_filter_test.cpp
  )
  target_link_libraries(
    barely_one_pole_filter_test
    barely_one_pole_filter
    gtest_main
  )
  gtest_discover_tests(barely_one_pole_filter_test)

  add_executable(
    barely_oscillator_test
    oscillator_test.cpp
  )
  target_link_libraries(
    barely_oscillator_test
    barely_oscillator
    gtest_main
  )
  gtest_discover_tests(barely_oscillator_test)

  add_executable(
    barely_polyphonic_voice_test
    polyphonic_voice_test.cpp
  )
  target_link_libraries(
    barely_polyphonic_voice_test
    barely_polyphonic_voice
    gtest_main
  )
  gtest_discover_tests(barely_polyphonic_voice_test)

  add_executable(
    barely_sample_player_test
    sample_player_test.cpp
  )
  target_link_libraries(
    barely_sample_player_test
    barely_sample_player
    gtest_main
  )
  gtest_discover_tests(barely_sample_player_test)
endif()
