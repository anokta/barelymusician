cmake_minimum_required(VERSION 3.14)

project(barelymusician)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN true)

if (MSVC)
  add_compile_options(
    /W4
    /WX
  )
elseif (APPLE)
  add_compile_options(
    -Wall
    -Wextra
    -Wno-missing-field-initializers
    -Wno-unused-lambda-capture
    -Werror
  )
else ()
  add_compile_options(
    -Wall
    -Wextra
    -Wno-missing-field-initializers
    -Werror
  )
endif ()

include(FetchContent)

enable_testing()
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929
)
if (MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

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

include_directories(.)

add_subdirectory(barelymusician)
add_subdirectory(examples)
add_subdirectory(unity)
