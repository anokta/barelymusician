add_subdirectory(common)
add_subdirectory(engine)
add_subdirectory(instruments)

add_library(
  barelymusician
  barelymusician.cpp
)
target_compile_definitions(
  barelymusician
  PUBLIC BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusician
  include
  engine
  percussion_instrument
  synth_instrument
)

add_library(
  include INTERFACE
  barelymusician.h
)
