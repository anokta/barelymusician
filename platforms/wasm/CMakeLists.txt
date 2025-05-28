set(CMAKE_FOLDER platforms)

add_executable(
  barelymusicianwasm
  wasm.cpp
)
set_target_properties(
  barelymusicianwasm PROPERTIES
  LINK_FLAGS "--bind \
    -s SINGLE_FILE=1 \
    -s ENVIRONMENT=web \
    -s WASM=1 \
    -s NO_FILESYSTEM=1 \
    -s BINARYEN_ASYNC_COMPILATION=0 \
    -s ASSERTIONS=0 \
    -s EXPORT_ES6=1 \
    -s MODULARIZE=1 \
    -s EXPORTED_FUNCTIONS=['_malloc','_free'] \
    -s EXPORT_ALL=1 \
    -s EXPORT_NAME=barelymusician"
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
