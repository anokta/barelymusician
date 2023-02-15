add_library(
  custom_instrument INTERFACE
  custom_instrument.h
)
target_link_libraries(
  custom_instrument INTERFACE
  barelymusician
)

add_library(
  percussion_instrument
  percussion_instrument.cpp
  percussion_instrument.h
)
target_compile_definitions(
  percussion_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  percussion_instrument
  custom_instrument
  barelymusician
  enveloped_voice
  gain_processor
  sample_player
)

add_library(
  sampler_instrument
  sampler_instrument.cpp
  sampler_instrument.h
)
target_compile_definitions(
  sampler_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  sampler_instrument
  custom_instrument
  barelymusician
  enveloped_voice
  gain_processor
  polyphonic_voice
  sample_player
)

add_library(
  synth_instrument
  synth_instrument.cpp
  synth_instrument.h
)
target_compile_definitions(
  synth_instrument
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  synth_instrument
  custom_instrument
  barelymusician
  dsp_utils
  enveloped_voice
  gain_processor
  oscillator
  polyphonic_voice
)
