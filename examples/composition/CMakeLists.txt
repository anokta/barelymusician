add_library(
  barelymusician_examples_composition
  repeater.cpp
  repeater.h
)
target_link_libraries(
  barelymusician_examples_composition PRIVATE
  barelymusician
)
