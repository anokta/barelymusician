add_library(
  engine
  engine.cpp
  engine.h
)
target_link_libraries(
  engine
  fixed
  id
  instrument
  mutable_data
  sequencer
  find_or_null
)

add_library(
  fixed
  fixed.cpp
  fixed.h
)

add_library(
  id INTERFACE
  id.h
)
target_link_libraries(
  id INTERFACE
  include
)

add_library(
  instrument
  instrument.cpp
  instrument.h
)
target_link_libraries(
  instrument
  message
  message_queue
  parameter
  include
)

add_library(
  message INTERFACE
  message.h
)

add_library(
  message_queue
  message_queue.cpp
  message_queue.h
)
target_link_libraries(
  message_queue
  message
)

add_library(
  mutable_data INTERFACE
  mutable_data.h
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

add_library(
  sequencer
  sequencer.cpp
  sequencer.h
)
target_link_libraries(
  sequencer
  fixed
  id
  find_or_null
)

add_executable(
  engine_test
  engine_test.cpp
)
target_link_libraries(
  engine_test
  engine
  id
  instrument
  parameter
  gmock
  gtest_main
)
gtest_discover_tests(engine_test)

add_executable(
  instrument_test
  instrument_test.cpp
)
target_link_libraries(
  instrument_test
  instrument
  parameter
  gmock
  gtest_main
)
gtest_discover_tests(instrument_test)

add_executable(
  message_queue_test
  message_queue_test.cpp
)
target_link_libraries(
  message_queue_test
  message
  message_queue
  gmock
  gtest_main
)
gtest_discover_tests(message_queue_test)

add_executable(
  mutable_data_test
  mutable_data_test.cpp
)
target_link_libraries(
  mutable_data_test
  mutable_data
  gmock
  gtest_main
)
gtest_discover_tests(mutable_data_test)

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

add_library(
  sequencer_test
  sequencer_test.cpp
)
target_link_libraries(
  sequencer_test
  id
  sequencer
  gmock
  gtest_main
)
