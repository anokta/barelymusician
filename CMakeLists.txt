cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED ENABLE_EXAMPLES)
  set(ENABLE_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
endif()
option(ENABLE_EXAMPLES "Build examples" ${ENABLE_EXAMPLES})

if(NOT DEFINED ENABLE_DAISY)
  set(ENABLE_DAISY OFF CACHE BOOL "Build Daisy platform" FORCE)
endif()
option(ENABLE_DAISY "Build Daisy platform" ${ENABLE_DAISY})

if(NOT DEFINED ENABLE_UNITY)
  set(ENABLE_UNITY OFF CACHE BOOL "Build Unity platform" FORCE)
endif()
option(ENABLE_UNITY "Build Unity platform" ${ENABLE_UNITY})

if(NOT DEFINED ENABLE_VST)
  set(ENABLE_VST OFF CACHE BOOL "Build VST platform" FORCE)
endif()
option(ENABLE_VST "Build VST platform" ${ENABLE_VST})

if(NOT DEFINED ENABLE_BENCHMARKS)
  set(ENABLE_BENCHMARKS OFF CACHE BOOL "Build benchmarks" FORCE)
endif()
option(ENABLE_BENCHMARKS "Build benchmarks" ${ENABLE_BENCHMARKS})

if(NOT DEFINED ENABLE_TESTS)
  set(ENABLE_TESTS OFF CACHE BOOL "Build tests" FORCE)
endif()
option(ENABLE_TESTS "Build tests" ${ENABLE_TESTS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

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
  set_target_properties(
    daisy PROPERTIES
    FOLDER external
  )
  target_compile_definitions(
    daisy PUBLIC
    BOOT_APP # Use the Daisy bootloader.
  )
  target_compile_options(
    daisy PUBLIC
    -w
  )
  include_directories(${libdaisy_SOURCE_DIR}/src)
endif()

project(
  barelymusician
  VERSION 0.6.0
  DESCRIPTION "a real-time music engine for interactive systems"
  HOMEPAGE_URL "https://github.com/anokta/barelymusician"
)

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

if(ENABLE_ASAN)
  if(MSVC)
    add_compile_options(
      /fsanitize=address
      /MTd
    )
  else()
    add_compile_options(
      -fsanitize=address
      -fno-omit-frame-pointer
    )
    add_link_options(
      -fsanitize=address
    )
  endif()
endif()

if(ENABLE_MSAN)
  if(NOT MSVC)
    add_compile_options(
      -fsanitize=memory
      -fno-omit-frame-pointer
    )
    add_link_options(
      -fsanitize=memory
    )
  endif()
endif()

if(ENABLE_TSAN)
  if(NOT MSVC)
    add_compile_options(
      -fsanitize=thread
      -fno-omit-frame-pointer
    )
    add_link_options(
      -fsanitize=thread
    )
  endif()
endif()

if(ENABLE_BENCHMARKS)
  set(CMAKE_FOLDER external)
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.0
  )
  set(BENCHMARK_ENABLE_TESTING OFF)
  FetchContent_MakeAvailable(benchmark)
  target_compile_options(
    benchmark PUBLIC
    -w
  )
  include_directories(${benchmark_SOURCE_DIR}/include)
endif()

if(ENABLE_TESTS)
  enable_testing()
  set(CMAKE_FOLDER external)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
  )
  if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()
  FetchContent_MakeAvailable(googletest)
  include(GoogleTest)
endif()

include_directories(include)

add_subdirectory(src)
add_subdirectory(platforms)

if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
