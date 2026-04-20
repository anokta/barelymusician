function(add_demo source_file)
  get_filename_component(name ${source_file} NAME_WE)
  set(DEMO_TARGET_NAME barelymusician_examples_demo_${name})
  add_executable(
    ${DEMO_TARGET_NAME}
    ${source_file}
  )
  set_target_properties(
    ${DEMO_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${name}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples
  )
  get_filename_component(ext ${source_file} EXT)
  if(ext STREQUAL ".c")
    set_target_properties(
      ${DEMO_TARGET_NAME} PROPERTIES
      C_STANDARD_REQUIRED ON
      C_EXTENSIONS OFF
    )
    target_link_libraries(
      ${DEMO_TARGET_NAME} PRIVATE
      barelymusician
      ${ARGN}
    )
  else()
    target_link_libraries(
      ${DEMO_TARGET_NAME} PRIVATE
      barelymusician
      barelymusician_examples_common
      barelymusician_examples_composition
      barelymusician_examples_data
      ${ARGN}
    )
  endif()
  add_dependencies(barelymusician_examples ${DEMO_TARGET_NAME})
endfunction()
add_custom_target(barelymusician_examples)
add_demo(c_demo.c miniaudio)
add_demo(instrument_demo.cpp)
add_demo(metronome_demo.cpp)
add_demo(midi_demo.cpp midifile)
add_demo(musician_demo.cpp)
add_demo(repeater_demo.cpp)
add_demo(sampler_demo.cpp)
add_demo(sequencer_demo.cpp)
add_demo(trigger_demo.cpp)
