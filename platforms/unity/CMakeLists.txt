add_library(
  barelymusicianunity
  empty.cpp
)
target_compile_definitions(
  barelymusicianunity
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusicianunity PUBLIC
  barelymusician
  barely_arpeggiator
  barely_high_pass_effect
  barely_lfo_effect
  barely_low_pass_effect
  barely_percussion_instrument
  barely_sampler_instrument
  barely_synth_instrument
)
if (MSVC)
  target_sources(
    barelymusicianunity PUBLIC
    unity_windows.def
  )
endif ()
