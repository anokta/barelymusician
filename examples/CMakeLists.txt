set(CMAKE_FOLDER external)

FetchContent_Declare(
  dr_libs
  GIT_REPOSITORY https://github.com/mackron/dr_libs.git
  GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01
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
  GIT_TAG f79e8ea395b858ceb6e0d8f353885ff80773dc76
)
FetchContent_MakeAvailable(midifile)
target_compile_options(
  midifile PUBLIC
  -w
)
include_directories(${midifile_SOURCE_DIR}/include)

FetchContent_Declare(
  miniaudio
  GIT_REPOSITORY https://github.com/mackron/miniaudio.git
  GIT_TAG 4a5b74bef029b3592c54b6048650ee5f972c1a48
)
FetchContent_MakeAvailable(miniaudio)
add_library(
  miniaudio INTERFACE
  ${miniaudio_SOURCE_DIR}/miniaudio.h
)
target_compile_options(
  miniaudio INTERFACE
  -w
)
include_directories(${miniaudio_SOURCE_DIR})

set(CMAKE_FOLDER examples)

include_directories(.)

add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(demo)
add_subdirectory(performers)
