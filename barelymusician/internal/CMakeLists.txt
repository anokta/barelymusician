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
  barelymusician_headeronly
)

add_library(
  barely_internal_engine
  engine.cpp
  engine.h
)
target_link_libraries(
  barely_internal_engine
  barely_internal_id
  barely_internal_instrument
  barely_internal_mutable_data
  barely_internal_performer
  barely_internal_status_or
  barelymusician_headeronly
  barely_find_or_null
  barely_seconds
)

add_library(
  barely_internal_event INTERFACE
  event.h
)

add_library(
  barely_internal_id INTERFACE
  id.h
)
target_link_libraries(
  barely_internal_id INTERFACE
  barelymusician_headeronly
)

add_library(
  barely_internal_instrument
  instrument.cpp
  instrument.h
)
target_link_libraries(
  barely_internal_instrument
  barely_internal_control
  barely_internal_effect
  barely_internal_event
  barely_internal_id
  barely_internal_message
  barely_internal_message_queue
  barely_internal_mutable_data
  barely_internal_status_or
  barelymusician_headeronly
  barely_find_or_null
  barely_seconds
)

add_library(
  barely_internal_message INTERFACE
  message.h
)
target_link_libraries(
  barely_internal_message INTERFACE
  barely_internal_id
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
  barely_internal_mutable_data INTERFACE
  mutable_data.h
)

add_library(
  barely_internal_performer
  performer.cpp
  performer.h
)
target_link_libraries(
  barely_internal_performer
  barely_internal_event
  barely_internal_id
  barely_internal_status_or
  barelymusician_headeronly
  barely_find_or_null
)

add_library(
  barely_internal_status_or INTERFACE
  status_or.h
)
target_link_libraries(
  barely_internal_status_or INTERFACE
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
    barely_internal_engine_test
    engine_test.cpp
  )
  target_link_libraries(
    barely_internal_engine_test
    barely_internal_control
    barely_internal_engine
    barely_internal_id
    barely_internal_instrument
    barely_internal_status_or
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_engine_test)

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
    barely_internal_status_or
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
    barely_internal_mutable_data_test
    mutable_data_test.cpp
  )
  target_link_libraries(
    barely_internal_mutable_data_test
    barely_internal_mutable_data
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_mutable_data_test)

  add_executable(
    barely_internal_performer_test
    performer_test.cpp
  )
  target_link_libraries(
    barely_internal_performer_test
    barely_internal_id
    barely_internal_performer
    barely_internal_status_or
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_internal_performer_test)

  add_executable(
    barely_internal_status_or_test
    status_or_test.cpp
  )
  target_link_libraries(
    barely_internal_status_or_test
    barely_internal_status_or
    gtest_main
  )
  gtest_discover_tests(barely_internal_status_or_test)
endif()
