cmake_minimum_required(VERSION 3.20)

unset(ENABLE_EXAMPLES CACHE)
unset(ENABLE_TESTS CACHE)
unset(ENABLE_DAISY CACHE)
unset(ENABLE_UNITY CACHE)

option(ENABLE_EXAMPLES "Build examples" OFF)
option(ENABLE_TESTS "Build tests" OFF)
option(ENABLE_DAISY "Build daisy platform" OFF)
option(ENABLE_UNITY "Build unity platform" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER predefined)

include(FetchContent)

if(ENABLE_DAISY)
  set(CMAKE_FOLDER external)
  FetchContent_Declare(
    libdaisy
    GIT_REPOSITORY https://github.com/electro-smith/libDaisy.git
    GIT_TAG 8d106a2c822f687bcb8c86bed0b6f2c22f680999
  )
  FetchContent_MakeAvailable(libdaisy)
  target_compile_definitions(
    daisy
    PUBLIC BOOT_APP # Use the Daisy bootloader.
  )
  target_compile_options(
    daisy
    PUBLIC -w
  )
  include_directories(${libdaisy_SOURCE_DIR}/src)
endif()

project(
  barelymusician
  VERSION 0.3.1
  DESCRIPTION "a real-time music engine for interactive systems"
  HOMEPAGE_URL "https://github.com/anokta/barelymusician"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(MSVC)
  add_compile_options(
    /W4
    /WX
  )
elseif(APPLE)
  add_compile_options(
    -Wall
    -Wextra
    -Wno-missing-field-initializers
    -Wno-unused-lambda-capture
    -Werror
  )
else()
  add_compile_options(
    -Wall
    -Wextra
    -Wno-missing-field-initializers
    -Werror
  )
endif()

if(ENABLE_TESTS)
  enable_testing()

  set(CMAKE_FOLDER external)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59
  )
  if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()
  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
endif()

include_directories(.)

add_subdirectory(barelymusician)

if(ENABLE_EXAMPLES)
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

  set(CMAKE_FOLDER external)
  FetchContent_Declare(
    midifile
    GIT_REPOSITORY https://github.com/craigsapp/midifile.git
    GIT_TAG f79e8ea395b858ceb6e0d8f353885ff80773dc76
  )
  FetchContent_MakeAvailable(midifile)
  target_compile_options(
    midifile
    PUBLIC -w
  )
  include_directories(${midifile_SOURCE_DIR}/include)

  set(CMAKE_FOLDER external)
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

  add_subdirectory(examples)
endif()

add_subdirectory(platforms)
