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
  percussion_instrument
  sampler_instrument
  synth_instrument
)
if (MSVC)
  target_sources(
    barelymusicianunity PUBLIC
    unity_windows.def
  )
endif ()
