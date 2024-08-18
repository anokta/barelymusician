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
  barelymusician_arpeggiator
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
)

add_barelymusician_demo(
  instrument_demo
  barelymusician
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
)

add_barelymusician_demo(
  metronome_demo
  barelymusician
  barelymusician_metronome
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
)

add_barelymusician_demo(
  midi_demo
  barelymusician
  barelymusician_midi
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
  barelymusician_examples_data
  midifile
)

add_barelymusician_demo(
  musician_demo
  barelymusician
  barelymusician_random
  barelymusician_metronome
  barelymusician_duration
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_percussion_instrument
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
  barelymusician_examples_wav_file
  barelymusician_examples_data
)

add_barelymusician_demo(
  repeater_demo
  barelymusician
  barelymusician_repeater
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
)

add_barelymusician_demo(
  sampler_demo
  barelymusician
  barelymusician_scale
  barelymusician_low_pass_effect
  barelymusician_sampler_instrument
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
  barelymusician_examples_wav_file
  barelymusician_examples_data
)

add_barelymusician_demo(
  sequencer_demo
  barelymusician
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
)

add_barelymusician_demo(
  trigger_demo
  barelymusician
  barelymusician_scale
  barelymusician_oscillator
  barelymusician_synth_instrument
  barelymusician_examples_audio_clock
  barelymusician_examples_audio_output
  barelymusician_examples_console_log
  barelymusician_examples_input_manager
)

