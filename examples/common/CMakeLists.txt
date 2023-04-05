add_library(
  audio_clock
  audio_clock.cpp
  audio_clock.h
)

add_library(
  audio_output
  audio_output.cpp
  audio_output.h
)
target_link_libraries(
  audio_output
  portaudio_static
)

add_library(
  console_log INTERFACE
  console_log.h
)

add_library(
  input_manager
  input_manager.cpp
  input_manager.h
)
if (MSVC)
  target_link_options(
    input_manager
    PUBLIC -l user32.lib
  )
elseif (APPLE)
  target_link_options(
    input_manager
    PUBLIC -framework ApplicationServices
  )
endif ()
