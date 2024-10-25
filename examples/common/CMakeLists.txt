add_library(
  examples_common_audio_clock
  audio_clock.cpp
  audio_clock.h
)

add_library(
  examples_common_audio_output
  audio_output.cpp
  audio_output.h
)
target_link_libraries(
  examples_common_audio_output
  miniaudio
)

add_library(
  examples_common_console_log INTERFACE
  console_log.h
)

add_library(
  examples_common_input_manager
  input_manager.cpp
  input_manager.h
)
if(MSVC)
  target_link_options(
    examples_common_input_manager
    PUBLIC -l user32.lib
  )
elseif(APPLE)
  target_link_options(
    examples_common_input_manager
    PUBLIC -framework ApplicationServices
  )
endif()

add_library(
  examples_common_wav_file
  wav_file.cpp
  wav_file.h
)
target_link_libraries(
  examples_common_wav_file
  dr_wav
)
