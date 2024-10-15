add_library(
  barelymusician_internal_control
  control.cpp
  control.h
)
target_link_libraries(
  barelymusician_internal_control
  barelymusician_internal_event
  barelymusician_headeronly
)

add_library(
  barelymusician_internal_event INTERFACE
  event.h
)

add_library(
  barelymusician_internal_instrument_controller
  instrument_controller.cpp
  instrument_controller.h
  instrument_processor.cpp
  instrument_processor.h
)
target_link_libraries(
  barelymusician_internal_instrument_controller
  barelymusician_internal_control
  barelymusician_internal_event
  barelymusician_internal_message
  barelymusician_internal_message_queue
  barelymusician_internal_sample_data
  barelymusician_headeronly
  barelymusician_find_or_null
  barelymusician_gain_processor
  barelymusician_oscillator
  barelymusician_polyphonic_voice
)

add_library(
  barelymusician_internal_message INTERFACE
  message.h
)
target_link_libraries(
  barelymusician_internal_message INTERFACE
  barelymusician_internal_sample_data
)

add_library(
  barelymusician_internal_message_queue
  message_queue.cpp
  message_queue.h
)
target_link_libraries(
  barelymusician_internal_message_queue
  barelymusician_internal_message
)

add_library(
  barelymusician_internal_musician
  musician.cpp
  musician.h
)
target_link_libraries(
  barelymusician_internal_musician
  barelymusician_internal_instrument_controller
  barelymusician_internal_performer
)

add_library(
  barelymusician_internal_observable INTERFACE
  observable.h
)

add_library(
  barelymusician_internal_performer
  performer.cpp
  performer.h
)
target_link_libraries(
  barelymusician_internal_performer
  barelymusician_internal_task
  barelymusician_headeronly
)

add_library(
  barelymusician_internal_sample_data
  sample_data.cpp
  sample_data.h
)
target_link_libraries(
  barelymusician_internal_sample_data
  barelymusician_headeronly
)

add_library(
  barelymusician_internal_task
  task.cpp
  task.h
)
target_link_libraries(
  barelymusician_internal_task
  barelymusician_internal_event
  barelymusician_headeronly
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_internal_control_test
    control_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_control_test
    barelymusician_internal_control
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_control_test)

  add_executable(
    barelymusician_internal_event_test
    event_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_event_test
    barelymusician_internal_event
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_event_test)

  add_executable(
    barelymusician_internal_instrument_controller_test
    instrument_controller_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_instrument_controller_test
    barelymusician_internal_control
    barelymusician_internal_instrument_controller
    barelymusician_internal_sample_data
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_instrument_controller_test)

  add_executable(
    barelymusician_internal_message_queue_test
    message_queue_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_message_queue_test
    barelymusician_internal_message
    barelymusician_internal_message_queue
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_message_queue_test)

  add_executable(
    barelymusician_internal_musician_test
    musician_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_musician_test
    barelymusician_internal_control
    barelymusician_internal_instrument_controller
    barelymusician_internal_musician
    barelymusician_internal_performer
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_musician_test)

  add_executable(
    barelymusician_internal_observable_test
    observable_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_observable_test
    barelymusician_internal_observable
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_observable_test)

  add_executable(
    barelymusician_internal_performer_test
    performer_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_performer_test
    barelymusician_internal_performer
    barelymusician_internal_task
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_performer_test)

  add_executable(
    barelymusician_internal_sample_data_test
    sample_data_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_sample_data_test
    barelymusician_internal_sample_data
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_sample_data_test)

  add_executable(
    barelymusician_internal_task_test
    task_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_task_test
    barelymusician_internal_task
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_task_test)
endif()
