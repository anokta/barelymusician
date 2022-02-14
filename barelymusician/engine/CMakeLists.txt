add_library(
  instrument_controller
  instrument_controller.cpp
  instrument_controller.h
)
target_link_libraries(
  instrument_controller
  instrument_event
  parameter
  visitor
  header
)

add_library(
  instrument_event INTERFACE
  instrument_event.h
)

add_library(
  instrument_processor
  instrument_processor.cpp
  instrument_processor.h
)
target_link_libraries(
  instrument_processor
  instrument_event
  visitor
  header
)

add_library(
  parameter
  parameter.cpp
  parameter.h
)
target_link_libraries(
  parameter
  header
)

add_library(
  sequence
  sequence.cpp
  sequence.h
)
target_link_libraries(
  sequence
  instrument_event
  header
  find_or_null
)

add_library(
  task_runner
  task_runner.cpp
  task_runner.h
)

add_library(
  transport
  transport.cpp
  transport.h
)

add_library(
  visitor INTERFACE
  visitor.h
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

add_executable(
  task_runner_test
  task_runner_test.cpp
)
target_link_libraries(
  task_runner_test
  task_runner
  gtest_main
)
gtest_discover_tests(task_runner_test)

add_executable(
  transport_test
  transport_test.cpp
)
target_link_libraries(
  transport_test
  transport
  gmock
  gtest_main
)
gtest_discover_tests(transport_test)

add_executable(
  visitor_test
  visitor_test.cpp
)
target_link_libraries(
  visitor_test
  visitor
  gtest_main
)
gtest_discover_tests(visitor_test)
