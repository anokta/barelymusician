add_library(
  barely_examples_data
  data.cpp
  data.h
)
target_compile_definitions(
  barely_examples_data
  PRIVATE BARELYMUSICIAN_ROOT_DIR=\"${PROJECT_SOURCE_DIR}\"
)
