add_library(
  engine
  engine.cpp
)
target_link_libraries(
  engine
  instrument_controller
  instrument_event
  instrument_processor
  parameter
  sequence
  task_runner
  transport
  barelymusician
  find_or_null
  id
  status
)

add_library(
  instrument_controller
  instrument_controller.cpp
)
target_link_libraries(
  instrument_controller
  instrument_event
  parameter
  barelymusician
  visitor
)

add_library(
  instrument_event INTERFACE
)

add_library(
  instrument_processor
  instrument_processor.cpp
)
target_link_libraries(
  instrument_processor
  instrument_event
  barelymusician
  visitor
)

add_library(
  parameter
  parameter.cpp
)
target_link_libraries(
  parameter
  barelymusician
)

add_library(
  sequence
  sequence.cpp
)
target_link_libraries(
  sequence
  barelymusician
  find_or_null
  id
)

add_library(
  task_runner
  task_runner.cpp
)

add_library(
  transport
  transport.cpp
)

add_executable(
  engine_test
  engine_test.cpp
)
target_link_libraries(
  engine_test
  engine
  parameter
  barelymusician
  id
  status
  gmock
  gtest_main
)
gtest_discover_tests(engine_test)

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

