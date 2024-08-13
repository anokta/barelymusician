function(add_barelymusician_demo demo_name)
  set(DEMO_TARGET_NAME barelymusician_examples_${demo_name})
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
  barelymusician
  barely_arpeggiator
  barely_pitch
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
)

add_barelymusician_demo(
  instrument_demo
  barelymusician
  barely_pitch
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
)

add_barelymusician_demo(
  metronome_demo
  barelymusician
  barely_metronome
  barely_pitch
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
)

add_barelymusician_demo(
  midi_demo
  barelymusician
  barely_midi
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
  barely_examples_data
  midifile
)

add_barelymusician_demo(
  musician_demo
  barelymusician
  barely_random
  barely_metronome
  barely_duration
  barely_pitch
  barely_oscillator
  barely_percussion_instrument
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
  barely_examples_wav_file
  barely_examples_data
)

add_barelymusician_demo(
  repeater_demo
  barelymusician
  barely_repeater
  barely_pitch
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
)

add_barelymusician_demo(
  sampler_demo
  barelymusician
  barely_pitch
  barely_low_pass_effect
  barely_sampler_instrument
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
  barely_examples_wav_file
  barely_examples_data
)

add_barelymusician_demo(
  sequencer_demo
  barelymusician
  barely_midi
  barely_pitch
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
)

add_barelymusician_demo(
  trigger_demo
  barelymusician
  barely_midi
  barely_pitch
  barely_oscillator
  barely_synth_instrument
  barely_examples_audio_clock
  barely_examples_audio_output
  barely_examples_console_log
  barely_examples_input_manager
)

