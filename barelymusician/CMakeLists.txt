add_subdirectory(algorithm)
add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(composition)
add_subdirectory(dsp)
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
)

add_library(
  include INTERFACE
  barelymusician.h
)
