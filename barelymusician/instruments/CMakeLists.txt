add_library(
  barely_custom_instrument INTERFACE
  custom_instrument.h
)
target_link_libraries(
  barely_custom_instrument INTERFACE
  barelymusician
)

add_library(
  barely_percussion_instrument
  percussion_instrument.cpp
  percussion_instrument.h
)
target_compile_definitions(
  barely_percussion_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barely_percussion_instrument
  barely_custom_instrument
  barely_enveloped_voice
  barely_gain_processor
  barely_sample_player
)

add_library(
  barely_sampler_instrument
  sampler_instrument.cpp
  sampler_instrument.h
)
target_compile_definitions(
  barely_sampler_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barely_sampler_instrument
  barely_custom_instrument
  barely_enveloped_voice
  barely_gain_processor
  barely_polyphonic_voice
  barely_sample_player
)

add_library(
  barely_synth_instrument
  synth_instrument.cpp
  synth_instrument.h
)
target_compile_definitions(
  barely_synth_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barely_synth_instrument
  barely_custom_instrument
  barely_dsp_utils
  barely_enveloped_voice
  barely_gain_processor
  barely_oscillator
  barely_polyphonic_voice
)
