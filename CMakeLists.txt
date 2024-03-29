cmake_minimum_required(VERSION 3.14)

option(ENABLE_EXAMPLES "Build examples"  ON)
option(ENABLE_TESTS "Build tests" ON)

option(ENABLE_DAISY "Build daisy platform" OFF)
option(ENABLE_UNITY "Build unity platform" ON)

include(FetchContent)

if (ENABLE_DAISY)
  FetchContent_Declare(
    libdaisy
    GIT_REPOSITORY https://github.com/electro-smith/libDaisy.git
    GIT_TAG ae9b45e2927aafba5f261f2ff36e3f41ae74d019
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

project(barelymusician)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

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

if (ENABLE_TESTS)
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
endif()

include_directories(.)

add_subdirectory(barelymusician)

if (ENABLE_EXAMPLES)
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

  add_subdirectory(examples)
endif()

if (ENABLE_DAISY)
  add_subdirectory(platforms/daisy)
endif()

if (ENABLE_UNITY)
  add_subdirectory(platforms/unity)
endif()
