add_library(
  barelymusician_custom_instrument INTERFACE
  custom_instrument.h
)
target_link_libraries(
  barelymusician_custom_instrument INTERFACE
  barelymusician
)

add_library(
  barelymusician_percussion_instrument
  percussion_instrument.cpp
  percussion_instrument.h
)
target_compile_definitions(
  barelymusician_percussion_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_percussion_instrument
  barelymusician_custom_instrument
  barelymusician_enveloped_voice
  barelymusician_gain_processor
  barelymusician_sample_player
)

add_library(
  barelymusician_sampler_instrument
  sampler_instrument.cpp
  sampler_instrument.h
)
target_compile_definitions(
  barelymusician_sampler_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_sampler_instrument
  barelymusician_custom_instrument
  barelymusician_enveloped_voice
  barelymusician_gain_processor
  barelymusician_polyphonic_voice
  barelymusician_sample_player
)

add_library(
  barelymusician_synth_instrument
  synth_instrument.cpp
  synth_instrument.h
)
target_compile_definitions(
  barelymusician_synth_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_synth_instrument
  barelymusician_custom_instrument
  barelymusician_enveloped_voice
  barelymusician_gain_processor
  barelymusician_oscillator
  barelymusician_polyphonic_voice
)

add_library(
  barelymusician_ultimate_instrument
  ultimate_instrument.cpp
  ultimate_instrument.h
)
target_compile_definitions(
  barelymusician_ultimate_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_ultimate_instrument
  barelymusician_custom_instrument
  barelymusician_enveloped_voice
  barelymusician_gain_processor
  barelymusician_oscillator
  barelymusician_polyphonic_voice
)
