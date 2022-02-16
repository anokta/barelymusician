cmake_minimum_required(VERSION 3.10)

project(barelymusician)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

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
if (MSVC)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

include_directories(.)

add_subdirectory(barelymusician)
