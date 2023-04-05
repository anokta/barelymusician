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

enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_Declare(
  portaudio
  URL http://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz
  URL_HASH  MD5=ad319249932c6794b551d954b8844402
)
if (MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(portaudio)
include(GoogleTest)

include_directories(.)

add_subdirectory(barelymusician)
add_subdirectory(examples)
add_subdirectory(unity)
