add_library(
  examples_data_data
  data.cpp
  data.h
)
target_compile_definitions(
  examples_data_data
  PRIVATE BARELYMUSICIAN_EXAMPLES_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
)
