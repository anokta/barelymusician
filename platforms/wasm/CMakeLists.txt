set(CMAKE_FOLDER platforms)

add_executable(
  barelymusicianwasm
  wasm.cpp
)
set_target_properties(
  barelymusicianwasm PROPERTIES
  LINK_FLAGS "--bind -s MODULARIZE=1 -s EXPORT_NAME=barelymusician"
  OUTPUT_NAME "barelymusician"
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_compile_definitions(
  barelymusicianwasm PUBLIC
  BARELYMUSICIAN_EXPORTS
)
target_link_libraries(
  barelymusicianwasm PRIVATE
  barelymusician
)
