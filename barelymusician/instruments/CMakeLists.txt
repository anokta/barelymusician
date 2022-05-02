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
  instrument
  parameter
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
  sample_player
  instrument
  parameter
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
  dsp_utils
  oscillator
  polyphonic_voice
  instrument
  parameter
)
