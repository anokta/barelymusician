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
  event_queue
  event_queue.cpp
  event_queue.h
)
target_link_libraries(
  event_queue
  event
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
  event_queue_test
  event_queue_test.cpp
)
target_link_libraries(
  event_queue_test
  event
  event_queue
  gtest_main
)
gtest_discover_tests(event_queue_test)

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
