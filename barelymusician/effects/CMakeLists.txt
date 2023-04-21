add_library(
  custom_effect INTERFACE
  custom_effect.h
)
target_link_libraries(
  custom_effect INTERFACE
  barelymusician
)

add_library(
  high_pass_effect
  high_pass_effect.cpp
  high_pass_effect.h
)
target_compile_definitions(
  high_pass_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  high_pass_effect
  custom_effect
  barelymusician
  dsp_utils
  one_pole_filter
)

add_library(
  low_pass_effect
  low_pass_effect.cpp
  low_pass_effect.h
)
target_compile_definitions(
  low_pass_effect
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  low_pass_effect
  custom_effect
  barelymusician
  dsp_utils
  one_pole_filter
)
