add_library(
  enveloped_voice INTERFACE
  enveloped_voice.h
)
target_link_libraries(
  enveloped_voice INTERFACE
  envelope
  voice
)

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
  enveloped_voice
  generic_instrument
  barelymusician
  sample_player
)

add_library(
  sampler_instrument
  sampler_instrument.cpp
  sampler_instrument.h
)
target_link_libraries(
  sampler_instrument
  enveloped_voice
  generic_instrument
  barelymusician
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
  enveloped_voice
  generic_instrument
  barelymusician
  dsp_utils
  oscillator
  polyphonic_voice
)
