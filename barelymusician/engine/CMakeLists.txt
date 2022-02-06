add_library(
  conductor
  conductor.cpp
)
target_link_libraries(
  conductor
  conductor_definition
  parameter
  parameter_definition
  find_or_null
  status
  note_duration
  note_intensity
  note_pitch
)

add_library(
  conductor_definition INTERFACE
)
target_link_libraries(
  conductor_definition INTERFACE
  parameter_definition
  status
  note_duration
  note_intensity
  note_pitch
)

add_library(
  engine
  engine.cpp
)
target_link_libraries(
  engine
  conductor
  conductor_definition
  instrument_controller
  instrument_definition
  instrument_event
  instrument_processor
  parameter
  parameter_definition
  task_runner
  transport
  find_or_null
  id
  id_generator
  status
  sequence
  dsp_utils
)

add_library(
  instrument_controller
  instrument_controller.cpp
)
target_link_libraries(
  instrument_controller
  instrument_definition
  instrument_event
  parameter
  parameter_definition
  visitor
)

add_library(
  instrument_definition INTERFACE
)
target_link_libraries(
  instrument_definition INTERFACE
  parameter_definition
)

add_library(
  instrument_event INTERFACE
)
target_link_libraries(
  instrument_event INTERFACE
  instrument_definition
)

add_library(
  instrument_processor
  instrument_processor.cpp
)
target_link_libraries(
  instrument_processor
  instrument_definition
  instrument_event
  visitor
  dsp_utils
)

add_library(
  parameter
  parameter.cpp
)
target_link_libraries(
  parameter
  parameter_definition
)

add_library(
  parameter_definition INTERFACE
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
  conductor_test
  conductor_test.cpp
)
target_link_libraries(
  conductor_test
  conductor
  conductor_definition
  parameter_definition
  status
  note_duration
  note_intensity
  note_pitch
  gtest_main
)
gtest_discover_tests(conductor_test)

add_executable(
  engine_test
  engine_test.cpp
)
target_link_libraries(
  engine_test
  engine
  instrument_definition
  parameter
  parameter_definition
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
  parameter_definition
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

