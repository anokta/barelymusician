add_library(
  data
  data.cpp
  data.h
)
target_compile_definitions(
  data
  PRIVATE BARELYMUSICIAN_ROOT_DIR=\"${PROJECT_SOURCE_DIR}\"
)
