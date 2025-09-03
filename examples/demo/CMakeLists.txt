function(add_demo demo_name)
  set(DEMO_TARGET_NAME barelymusician_examples_demo_${demo_name})
  add_executable(
    ${DEMO_TARGET_NAME}
    ${demo_name}.cpp
  )
  set_target_properties(
    ${DEMO_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${demo_name}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
  )
  target_link_libraries(
    ${DEMO_TARGET_NAME} PRIVATE
    barelymusician
    barelymusician_examples_common
    barelymusician_examples_composition
    barelymusician_examples_data
    ${ARGN}
  )
endfunction()
add_demo(arpeggiator_demo)
add_demo(instrument_demo)
add_demo(metronome_demo)
add_demo(midi_demo midifile)
add_demo(musician_demo)
add_demo(repeater_demo)
add_demo(sampler_demo)
add_demo(sequencer_demo)
add_demo(trigger_demo)
