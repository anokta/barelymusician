add_library(
  barelymusician_examples_common
  audio_clock.h
  audio_output.cpp
  audio_output.h
  console_log.h
  input_manager.cpp
  input_manager.h
  wav_file.cpp
  wav_file.h
)
target_link_libraries(
  barelymusician_examples_common
  dr_wav
  miniaudio
)
if(MSVC)
  target_link_options(
    barelymusician_examples_common
    PUBLIC -l user32.lib
  )
elseif(APPLE)
  target_link_options(
    barelymusician_examples_common
    PUBLIC -framework ApplicationServices
  )
endif()
