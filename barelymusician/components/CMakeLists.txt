add_library(
  barely_arpeggiator
  arpeggiator.cpp
  arpeggiator.h
)
target_link_libraries(
  barely_arpeggiator
  barelymusician
  barely_random
)

add_library(
  barely_metronome
  metronome.cpp
  metronome.h
)
target_link_libraries(
  barely_metronome
  barelymusician
)

add_library(
  barely_repeater
  repeater.cpp
  repeater.h
)
target_link_libraries(
  barely_repeater
  barelymusician
  barely_random
)
