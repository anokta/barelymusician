add_library(
  barely_custom_effect INTERFACE
  custom_effect.h
)
target_link_libraries(
  barely_custom_effect INTERFACE
  barelymusician
)

add_library(
  barely_high_pass_effect
  high_pass_effect.cpp
  high_pass_effect.h
)
target_compile_definitions(
  barely_high_pass_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barely_high_pass_effect
  barely_custom_effect
  barelymusician
  barely_dsp_utils
  barely_one_pole_filter
)

add_library(
  barely_lfo_effect
  lfo_effect.cpp
  lfo_effect.h
)
target_compile_definitions(
  barely_lfo_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barely_lfo_effect
  barely_custom_effect
  barelymusician
  barely_oscillator
)


add_library(
  barely_low_pass_effect
  low_pass_effect.cpp
  low_pass_effect.h
)
target_compile_definitions(
  barely_low_pass_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barely_low_pass_effect
  barely_custom_effect
  barelymusician
  barely_dsp_utils
  barely_one_pole_filter
)
