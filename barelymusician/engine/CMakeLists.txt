add_library(
  data
  data.cpp
  data.h
)
target_link_libraries(
  data
  include
)

add_library(
  event INTERFACE
  event.h
)
target_link_libraries(
  event
  data
)

add_library(
  parameter
  parameter.cpp
  parameter.h
)
target_link_libraries(
  parameter
  include
)

add_executable(
  parameter_test
  parameter_test.cpp
)
target_link_libraries(
  parameter_test
  parameter
  gtest_main
)
gtest_discover_tests(parameter_test)
