add_library(
  barelymusician_examples_data
  data.cpp
  data.h
)
target_compile_definitions(
  barelymusician_examples_data
  PRIVATE BARELYMUSICIAN_EXAMPLES_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
)
