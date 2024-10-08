add_library(
  barelymusician_custom_instrument INTERFACE
  custom_instrument.h
)
target_link_libraries(
  barelymusician_custom_instrument INTERFACE
  barelymusician
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
  barelymusician_sample_player
)
