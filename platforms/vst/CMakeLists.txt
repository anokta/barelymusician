set(CMAKE_FOLDER external)

FetchContent_Declare(
  vst3sdk
  GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
  GIT_TAG v3.7.13_build_42
)
FetchContent_MakeAvailable(vst3sdk)
target_compile_options(
  base PUBLIC
  -w
)
target_compile_options(
  pluginterfaces PUBLIC
  -w
)
target_compile_options(
  sdk PUBLIC
  -w
)

set(CMAKE_FOLDER platforms)

add_library(
  barelymusicianvst SHARED
  controller.cpp
  controller.h
  factory.cpp
  processor.cpp
  processor.h
)
set_target_properties(
  barelymusicianvst PROPERTIES
  OUTPUT_NAME barelymusician
  SUFFIX .vst3
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_compile_definitions(
  barelymusicianvst PUBLIC
  BARELY_EXPORT
)
target_include_directories(
  barelymusicianvst PRIVATE
  ${vst3sdk_SOURCE_DIR}
  ${vst3sdk_SOURCE_DIR}/base
  ${vst3sdk_SOURCE_DIR}/pluginterfaces
  ${vst3sdk_SOURCE_DIR}/public.sdk
)
target_link_libraries(
  barelymusicianvst PRIVATE
  barelymusician
  sdk
)
