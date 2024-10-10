add_library(
  barelymusician_decibels
  decibels.cpp
  decibels.h
)

add_library(
  barelymusician_envelope
  envelope.cpp
  envelope.h
)

add_library(
  barelymusician_enveloped_voice INTERFACE
  enveloped_voice.h
)
target_link_libraries(
  barelymusician_enveloped_voice INTERFACE
  barelymusician_envelope
  barelymusician_voice
)

add_library(
  barelymusician_gain_processor
  gain_processor.cpp
  gain_processor.h
)

add_library(
  barelymusician_one_pole_filter
  one_pole_filter.cpp
  one_pole_filter.h
)

add_library(
  barelymusician_oscillator
  oscillator.cpp
  oscillator.h
)
target_link_libraries(
  barelymusician_oscillator
  barelymusician_random
)

add_library(
  barelymusician_polyphonic_voice INTERFACE
  polyphonic_voice.h
)

add_library(
  barelymusician_sample_player
  sample_player.cpp
  sample_player.h
)

add_library(
  barelymusician_voice INTERFACE
  voice.h
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_decibels_test
    decibels_test.cpp
  )
  target_link_libraries(
    barelymusician_decibels_test
    barelymusician_decibels
    gtest_main
  )
  gtest_discover_tests(barelymusician_decibels_test)

  add_executable(
    barelymusician_envelope_test
    envelope_test.cpp
  )
  target_link_libraries(
    barelymusician_envelope_test
    barelymusician_envelope
    gtest_main
  )
  gtest_discover_tests(barelymusician_envelope_test)

  add_executable(
    barelymusician_gain_processor_test
    gain_processor_test.cpp
  )
  target_link_libraries(
    barelymusician_gain_processor_test
    barelymusician_gain_processor
    gtest_main
  )
  gtest_discover_tests(barelymusician_gain_processor_test)

  add_executable(
    barelymusician_one_pole_filter_test
    one_pole_filter_test.cpp
  )
  target_link_libraries(
    barelymusician_one_pole_filter_test
    barelymusician_one_pole_filter
    gtest_main
  )
  gtest_discover_tests(barelymusician_one_pole_filter_test)

  add_executable(
    barelymusician_oscillator_test
    oscillator_test.cpp
  )
  target_link_libraries(
    barelymusician_oscillator_test
    barelymusician_oscillator
    gtest_main
  )
  gtest_discover_tests(barelymusician_oscillator_test)

  add_executable(
    barelymusician_polyphonic_voice_test
    polyphonic_voice_test.cpp
  )
  target_link_libraries(
    barelymusician_polyphonic_voice_test
    barelymusician_polyphonic_voice
    gtest_main
  )
  gtest_discover_tests(barelymusician_polyphonic_voice_test)

  add_executable(
    barelymusician_sample_player_test
    sample_player_test.cpp
  )
  target_link_libraries(
    barelymusician_sample_player_test
    barelymusician_sample_player
    gtest_main
  )
  gtest_discover_tests(barelymusician_sample_player_test)
endif()
