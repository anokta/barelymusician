set(CMAKE_FOLDER platforms)

add_executable(
  barelymusicianwasm
  wasm.cpp
)
# TODO: Remove EXPORT_ALL.
set_target_properties(
  barelymusicianwasm PROPERTIES
  LINK_FLAGS "--bind \
    -s ASSERTIONS=0 \
    -s ENVIRONMENT=web,worker \
    -s EXPORT_ALL=1 \
    -s EXPORT_ES6=1 \
    -s EXPORTED_FUNCTIONS=['_malloc','_free'] \
    -s EXPORT_NAME=barelymusician \
    -s MODULARIZE=1 \
    -s NO_FILESYSTEM=1 \
    -s SINGLE_FILE=1 \
    -s WASM=1 \
    "
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
