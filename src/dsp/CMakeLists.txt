add_library(
  barelymusician_dsp_biquad_filter INTERFACE
  biquad_filter.h
)
target_link_libraries(
  barelymusician_dsp_biquad_filter INTERFACE
  barelymusician_headeronly
)

add_library(
  barelymusician_dsp_bit_crusher INTERFACE
  bit_crusher.h
)

add_library(
  barelymusician_dsp_decibels INTERFACE
  decibels.h
)

add_library(
  barelymusician_dsp_envelope INTERFACE
  envelope.h
)

add_library(
  barelymusician_dsp_gain_processor
  gain_processor.cpp
  gain_processor.h
)

add_library(
  barelymusician_dsp_instrument_processor
  instrument_processor.cpp
  instrument_processor.h
)
target_link_libraries(
  barelymusician_dsp_instrument_processor
  barelymusician_dsp_biquad_filter
  barelymusician_dsp_bit_crusher
  barelymusician_dsp_envelope
  barelymusician_dsp_gain_processor
  barelymusician_dsp_sample_data
  barelymusician_dsp_voice
  barelymusician_headeronly
)

add_library(
  barelymusician_dsp_oscillator INTERFACE
  oscillator.h
)

add_library(
  barelymusician_dsp_sample_data
  sample_data.cpp
  sample_data.h
)
target_link_libraries(
  barelymusician_dsp_sample_data
  barelymusician_headeronly
  barelymusician_private_random_impl
)

add_library(
  barelymusician_dsp_sample_player INTERFACE
  sample_player.h
)
target_link_libraries(
  barelymusician_dsp_sample_player INTERFACE
  barelymusician_headeronly
)

add_library(
  barelymusician_dsp_voice INTERFACE
  voice.h
)
target_link_libraries(
  barelymusician_dsp_voice INTERFACE
  barelymusician_dsp_biquad_filter
  barelymusician_dsp_bit_crusher
  barelymusician_dsp_envelope
  barelymusician_dsp_oscillator
  barelymusician_dsp_sample_player
  barelymusician_headeronly
  barelymusician_private_random_impl
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_dsp_biquad_filter_test
    biquad_filter_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_biquad_filter_test
    barelymusician_dsp_biquad_filter
    barelymusician
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_biquad_filter_test)

  add_executable(
    barelymusician_dsp_bit_crusher_test
    bit_crusher_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_bit_crusher_test
    barelymusician_dsp_bit_crusher
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_bit_crusher_test)

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
    barelymusician_dsp_instrument_processor_test
    instrument_processor_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_instrument_processor_test
    barelymusician_dsp_instrument_processor
    barelymusician_dsp_sample_data
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_instrument_processor_test)

  add_executable(
    barelymusician_dsp_oscillator_test
    oscillator_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_oscillator_test
    barelymusician_dsp_oscillator
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_oscillator_test)

  add_executable(
    barelymusician_dsp_sample_data_test
    sample_data_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_sample_data_test
    barelymusician_dsp_sample_data
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_sample_data_test)

  add_executable(
    barelymusician_dsp_sample_player_test
    sample_player_test.cpp
  )
  target_link_libraries(
    barelymusician_dsp_sample_player_test
    barelymusician_dsp_sample_player
    barelymusician
    gtest_main
  )
  gtest_discover_tests(barelymusician_dsp_sample_player_test)
endif()
