add_compile_definitions(BARELYMUSICIAN_CMAKE)

FetchContent_Declare(
  dr_libs
  GIT_REPOSITORY https://github.com/mackron/dr_libs.git
  GIT_TAG 4aff56541f5e6bd4b74053a0d5c9e6156e736059
)
FetchContent_MakeAvailable(dr_libs)
add_library(
  dr_wav INTERFACE
  ${dr_libs_SOURCE_DIR}/dr_wav.h
)
include_directories(${dr_libs_SOURCE_DIR})

FetchContent_Declare(
  midifile
  GIT_REPOSITORY https://github.com/craigsapp/midifile.git
  GIT_TAG de6aa0c8f82f9dff29b62dba013a65c9034d633d
)
FetchContent_MakeAvailable(midifile)
target_compile_options(
  midifile
  PUBLIC -w
)
include_directories(${midifile_SOURCE_DIR}/include)

FetchContent_Declare(
  portaudio
  GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
  GIT_TAG 147dd722548358763a8b649b3e4b41dfffbcfbb6
)
FetchContent_MakeAvailable(portaudio)
target_compile_options(
  portaudio
  PUBLIC -w
)
target_compile_options(
  portaudio_static
  PUBLIC -w
)

add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(demo)
