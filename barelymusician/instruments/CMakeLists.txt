add_library(
  generic_instrument INTERFACE
  generic_instrument.h
)
target_link_libraries(
  generic_instrument INTERFACE
  barelymusician
)

add_library(
  percussion_instrument
  percussion_instrument.cpp
  percussion_instrument.h
)
target_link_libraries(
  percussion_instrument
  generic_instrument
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
target_link_libraries(
  sampler_instrument
  generic_instrument
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
target_link_libraries(
  synth_instrument
  generic_instrument
  barelymusician
  dsp_utils
  enveloped_voice
  gain_processor
  oscillator
  polyphonic_voice
)
