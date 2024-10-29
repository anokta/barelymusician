function(add_barelymusician_demo demo_name)
  set(DEMO_TARGET_NAME examples_demo_${demo_name})
  add_executable(
    ${DEMO_TARGET_NAME}
    ${demo_name}.cpp
  )
  set_target_properties(
    ${DEMO_TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${demo_name}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )
  target_link_libraries(
    ${DEMO_TARGET_NAME} PRIVATE
    ${ARGN}
  )
endfunction()

add_barelymusician_demo(
  arpeggiator_demo
  barelycomposer
  barelymusician
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
)

add_barelymusician_demo(
  instrument_demo
  barelymusician
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
)

add_barelymusician_demo(
  metronome_demo
  barelymusician
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_performers_metronome
  examples_common_input_manager
)

add_barelymusician_demo(
  midi_demo
  barelymusician
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
  examples_data_data
  midifile
)

add_barelymusician_demo(
  musician_demo
  barelycomposer
  barelymusician
  barelymusician_composition_duration
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
  examples_performers_metronome
  examples_common_wav_file
  examples_data_data
)

add_barelymusician_demo(
  repeater_demo
  barelycomposer
  barelymusician
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
)

add_barelymusician_demo(
  sampler_demo
  barelymusician
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
  examples_common_wav_file
  examples_data_data
)

add_barelymusician_demo(
  sequencer_demo
  barelymusician
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
)

add_barelymusician_demo(
  trigger_demo
  barelycomposer
  barelymusician
  examples_common_audio_clock
  examples_common_audio_output
  examples_common_console_log
  examples_common_input_manager
)

