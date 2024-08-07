add_library(
  barely_internal_control
  control.cpp
  control.h
)
target_link_libraries(
  barely_internal_control
  barelymusician_headeronly
)

add_library(
  barely_internal_effect
  effect.cpp
  effect.h
)
target_link_libraries(
  barely_internal_effect
  barely_internal_control
  barely_internal_message
  barely_internal_message_queue
  barely_internal_seconds
  barelymusician_headeronly
  barely_find_or_null
)

add_library(
  barely_internal_event INTERFACE
  event.h
)

add_library(
  barely_internal_instrument
  instrument.cpp
  instrument.h
)
target_link_libraries(
  barely_internal_instrument
  barely_internal_control
  barely_internal_event
  barely_internal_message
  barely_internal_message_queue
  barely_internal_note
  barely_internal_seconds
  barelymusician_headeronly
  barely_find_or_null
)

add_library(
  barely_internal_message INTERFACE
  message.h
)

add_library(
  barely_internal_message_queue
  message_queue.cpp
  message_queue.h
)
target_link_libraries(
  barely_internal_message_queue
  barely_internal_message
)

add_library(
  barely_internal_musician
  musician.cpp
  musician.h
)
target_link_libraries(
  barely_internal_musician
  barely_internal_effect
  barely_internal_instrument
  barely_internal_performer
  barely_internal_seconds
  barelymusician_headeronly
)

add_library(
  barely_internal_note
  note.cpp
  note.h
)
target_link_libraries(
  barely_internal_note
  barely_internal_control
  barely_find_or_null
)

add_library(
  barely_internal_observable INTERFACE
  observable.h
)

add_library(
  barely_internal_performer
  performer.cpp
  performer.h
)
target_link_libraries(
  barely_internal_performer
  barely_internal_task
  barelymusician_headeronly
)

add_library(
  barely_internal_seconds
  seconds.cpp
  seconds.h
)

add_library(
  barely_internal_task
  task.cpp
  task.h
)
target_link_libraries(
  barely_internal_task
  barely_internal_event
  barelymusician_headeronly
)

if (ENABLE_TESTS)
  add_executable(
    barely_internal_control_test
    control_test.cpp
  )
  target_link_libraries(
    barely_internal_control_test
    barely_internal_control
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barely_internal_control_test)

  add_executable(
    barely_internal_event_test
    event_test.cpp
  )
  target_link_libraries(
    barely_internal_event_test
    barely_internal_event
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barely_internal_event_test)

  add_executable(
    barely_internal_instrument_test
    instrument_test.cpp
  )
  target_link_libraries(
    barely_internal_instrument_test
    barely_internal_control
    barely_internal_instrument
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_instrument_test)

  add_executable(
    barely_internal_message_queue_test
    message_queue_test.cpp
  )
  target_link_libraries(
    barely_internal_message_queue_test
    barely_internal_message
    barely_internal_message_queue
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_message_queue_test)

  add_executable(
    barely_internal_musician_test
    musician_test.cpp
  )
  target_link_libraries(
    barely_internal_musician_test
    barely_internal_control
    barely_internal_instrument
    barely_internal_musician
    barely_internal_performer
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_musician_test)

  add_executable(
    barely_internal_observable_test
    observable_test.cpp
  )
  target_link_libraries(
    barely_internal_observable_test
    barely_internal_observable
    gtest_main
  )
  gtest_discover_tests(barely_internal_observable_test)

  add_executable(
    barely_internal_performer_test
    performer_test.cpp
  )
  target_link_libraries(
    barely_internal_performer_test
    barely_internal_performer
    barely_internal_task
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_performer_test)

  add_executable(
    barely_internal_seconds_test
    seconds_test.cpp
  )
  target_link_libraries(
    barely_internal_seconds_test
    barely_internal_seconds
    gtest_main
  )
  gtest_discover_tests(barely_internal_seconds_test)

  add_executable(
    barely_internal_task_test
    task_test.cpp
  )
  target_link_libraries(
    barely_internal_task_test
    barely_internal_task
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_task_test)
endif()
