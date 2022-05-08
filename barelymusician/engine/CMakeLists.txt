add_library(
  clock
  clock.cpp
  clock.h
)

add_library(
  engine
  engine.cpp
  engine.h
)
target_link_libraries(
  engine
  clock
  id
  instrument
  mutable_data
  find_or_null
)

add_library(
  event INTERFACE
  event.h
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
  event
  event_queue
  parameter
  include
)

add_library(
  mutable_data INTERFACE
  mutable_data.h
)

add_library(
  note INTERFACE
  note.h
)
target_link_libraries(
  note INTERFACE
  include
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
  sequence
  sequence.cpp
  sequence.h
)
target_link_libraries(
  sequence
  clock
  id
  instrument
  note
  find_or_null
)

add_library(
  transport
  transport.cpp
  transport.h
)
target_link_libraries(
  transport
  include
)

add_executable(
  clock_test
  clock_test.cpp
)
target_link_libraries(
  clock_test
  clock
  gmock
  gtest_main
)
gtest_discover_tests(clock_test)

add_executable(
  engine_test
  engine_test.cpp
)
target_link_libraries(
  engine_test
  engine
  id
  instrument
  note
  parameter
  gmock
  gtest_main
)
gtest_discover_tests(engine_test)

add_executable(
  event_queue_test
  event_queue_test.cpp
)
target_link_libraries(
  event_queue_test
  event
  event_queue
  gmock
  gtest_main
)
gtest_discover_tests(event_queue_test)

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

add_executable(
  sequence_test
  sequence_test.cpp
)
target_link_libraries(
  sequence_test
  id
  instrument
  note
  sequence
  transport
  gmock
  gtest_main
)
gtest_discover_tests(sequence_test)

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
