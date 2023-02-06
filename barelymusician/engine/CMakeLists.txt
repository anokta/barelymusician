add_library(
  control
  control.cpp
  control.h
)
target_link_libraries(
  control
  include
)

add_library(
  engine
  engine.cpp
  engine.h
)
target_link_libraries(
  engine
  id
  instrument
  mutable_data
  number
  performer
  status
  task
  find_or_null
  dsp_utils
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
  control
  message
  message_queue
  status
  include
  find_or_null
  dsp_utils
)

add_library(
  message INTERFACE
  message.h
)
target_link_libraries(
  message
  number
)

add_library(
  message_queue
  message_queue.cpp
  message_queue.h
)
target_link_libraries(
  message_queue
  message
  number
)

add_library(
  mutable_data INTERFACE
  mutable_data.h
)

add_library(
  number INTERFACE
  number.h
)
target_link_libraries(
  number INTERFACE
  include
)

add_library(
  performer
  performer.cpp
  performer.h
)
target_link_libraries(
  performer
  id
  status
  task
  find_or_null
)

add_library(
  status INTERFACE
  status.h
)

add_library(
  task
  task.cpp
  task.h
)
target_link_libraries(
  task
  include
)

add_executable(
  control_test
  control_test.cpp
)
target_link_libraries(
  control_test
  control
  gtest_main
)
gtest_discover_tests(control_test)

add_executable(
  engine_test
  engine_test.cpp
)
target_link_libraries(
  engine_test
  control
  engine
  id
  instrument
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
  control
  instrument
  status
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
  number
  gmock
  gtest_main
)
gtest_discover_tests(mutable_data_test)

add_library(
  performer_test
  performer_test.cpp
)
target_link_libraries(
  performer_test
  id
  performer
  gtest_main
)
gtest_discover_tests(performer_test)

add_executable(
  task_test
  task_test.cpp
)
target_link_libraries(
  task_test
  number
  task
  gtest_main
)
gtest_discover_tests(task_test)
