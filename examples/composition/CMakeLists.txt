add_library(
  barelymusician_examples_composition
  arpeggiator.cpp
  arpeggiator.h
  repeater.cpp
  repeater.h
)
target_link_libraries(
  barelymusician_examples_composition PRIVATE
  barelymusician
)
