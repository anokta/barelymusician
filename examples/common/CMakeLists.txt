add_library(
  barelymusician_examples_audio_clock
  audio_clock.cpp
  audio_clock.h
)

add_library(
  barelymusician_examples_audio_output
  audio_output.cpp
  audio_output.h
)
target_link_libraries(
  barelymusician_examples_audio_output
  portaudio_static
)

add_library(
  barelymusician_examples_console_log INTERFACE
  console_log.h
)

add_library(
  barelymusician_examples_input_manager
  input_manager.cpp
  input_manager.h
)
if(MSVC)
  target_link_options(
    barelymusician_examples_input_manager
    PUBLIC -l user32.lib
  )
elseif(APPLE)
  target_link_options(
    barelymusician_examples_input_manager
    PUBLIC -framework ApplicationServices
  )
endif()

add_library(
  barelymusician_examples_wav_file
  wav_file.cpp
  wav_file.h
)
target_link_libraries(
  barelymusician_examples_wav_file
  dr_wav
)
