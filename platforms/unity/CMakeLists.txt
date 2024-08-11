add_library(
  barelymusicianunity SHARED
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
  barely_repeater
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
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  target_link_options(
    barelymusicianunity PUBLIC
    -ldl
    -lm
    "-Wl --version-script unity_android.lds"
    -z defs
  )
endif ()
