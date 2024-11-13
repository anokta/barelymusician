add_library(
  barelymusician_internal_event INTERFACE
  event.h
)

add_library(
  barelymusician_internal_instrument
  instrument.cpp
  instrument.h
)
target_link_libraries(
  barelymusician_internal_instrument
  barelymusician_internal_event
  barelymusician_internal_message
  barelymusician_internal_message_queue
  barelymusician_common_find_or_null
  barelymusician_dsp_instrument_processor
  barelymusician_dsp_one_pole_filter
  barelymusician_dsp_sample_data
  barelymusician_headeronly
)

add_library(
  barelymusician_internal_message INTERFACE
  message.h
)
target_link_libraries(
  barelymusician_internal_message INTERFACE
  barelymusician_dsp_sample_data
  barelymusician_headeronly
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
  barelymusician_internal_instrument
  barelymusician_internal_performer
  barelymusician_headeronly
)

add_library(
  barelymusician_internal_performer
  performer.cpp
  performer.h
)
target_link_libraries(
  barelymusician_internal_performer
  barelymusician_internal_event
  barelymusician_headeronly
)

if(ENABLE_TESTS)
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
    barelymusician_internal_instrument_test
    instrument_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_instrument_test
    barelymusician_internal_instrument
    barelymusician_dsp_sample_data
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_instrument_test)

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
    barelymusician_internal_instrument
    barelymusician_internal_musician
    barelymusician_internal_performer
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_musician_test)

  add_executable(
    barelymusician_internal_performer_test
    performer_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_performer_test
    barelymusician_internal_performer
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_performer_test)
endif()
