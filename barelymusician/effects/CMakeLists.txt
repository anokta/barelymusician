add_library(
  barelymusician_custom_effect INTERFACE
  custom_effect.h
)
target_link_libraries(
  barelymusician_custom_effect INTERFACE
  barelymusician
)

add_library(
  barelymusician_high_pass_effect
  high_pass_effect.cpp
  high_pass_effect.h
)
target_compile_definitions(
  barelymusician_high_pass_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_high_pass_effect
  barelymusician_custom_effect
  barelymusician
  barelymusician_one_pole_filter
)

add_library(
  barelymusician_lfo_effect
  lfo_effect.cpp
  lfo_effect.h
)
target_compile_definitions(
  barelymusician_lfo_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_lfo_effect
  barelymusician_custom_effect
  barelymusician
  barelymusician_oscillator
)


add_library(
  barelymusician_low_pass_effect
  low_pass_effect.cpp
  low_pass_effect.h
)
target_compile_definitions(
  barelymusician_low_pass_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician_low_pass_effect
  barelymusician_custom_effect
  barelymusician
  barelymusician_one_pole_filter
)
