add_library(
  conductor
  conductor.cpp
)
target_link_libraries(
  conductor
  conductor_definition
  param
  param_definition
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
  param_definition
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
  instrument_manager
  transport
  find_or_null
  id
  id_generator
  status
  sequence
)

add_library(
  instrument_controller
  instrument_controller.cpp
)
target_link_libraries(
  instrument_controller
  instrument_definition
  instrument_event
  param
  param_definition
  visitor
)

add_library(
  instrument_definition INTERFACE
)
target_link_libraries(
  instrument_definition INTERFACE
  param_definition
)

add_library(
  instrument_event INTERFACE
)
target_link_libraries(
  instrument_event INTERFACE
  instrument_definition
)

add_library(
  instrument_manager
  instrument_manager.cpp
)
target_link_libraries(
  instrument_manager
  instrument_controller
  instrument_definition
  instrument_event
  instrument_processor
  param
  param_definition
  task_runner
  find_or_null
  id
  status
  dsp_utils
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
  param
  param.cpp
)
target_link_libraries(
  param
  param_definition
)

add_library(
  param_definition INTERFACE
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
  param_definition
  status
  note_duration
  note_intensity
  note_pitch
  gtest_main
)
gtest_discover_tests(conductor_test)

add_executable(
  instrument_manager_test
  instrument_manager_test.cpp
)
target_link_libraries(
  instrument_manager_test
  instrument_definition
  instrument_manager
  param
  param_definition
  id
  status
  gmock
  gtest_main
)
gtest_discover_tests(instrument_manager_test)

add_executable(
  param_test
  param_test.cpp
)
target_link_libraries(
  param_test
  param
  param_definition
  gtest_main
)
gtest_discover_tests(param_test)

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

