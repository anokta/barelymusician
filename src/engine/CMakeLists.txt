add_library(
  barelymusician_engine_config INTERFACE
  config.h
)

add_library(
  barelymusician_engine_event INTERFACE
  event.h
)

add_library(
  barelymusician_engine_instrument
  instrument.cpp
  instrument.h
)
target_link_libraries(
  barelymusician_engine_instrument
  barelymusician_engine_event
  barelymusician_engine_message
  barelymusician_engine_message_queue
  barelymusician_common_find_or_null
  barelymusician_dsp_decibels
  barelymusician_dsp_instrument_processor
  barelymusician_dsp_one_pole_filter
  barelymusician_dsp_oscillator
  barelymusician_dsp_sample_data
  barelymusician_headeronly
)

add_library(
  barelymusician_engine_message INTERFACE
  message.h
)
target_link_libraries(
  barelymusician_engine_message INTERFACE
  barelymusician_dsp_sample_data
  barelymusician_headeronly
)

add_library(
  barelymusician_engine_message_queue
  message_queue.cpp
  message_queue.h
)
target_link_libraries(
  barelymusician_engine_message_queue
  barelymusician_engine_message
)

add_library(
  barelymusician_engine_musician
  musician.cpp
  musician.h
)
target_link_libraries(
  barelymusician_engine_musician
  barelymusician_engine_config
  barelymusician_engine_instrument
  barelymusician_engine_performer
  barelymusician_engine_pool
  barelymusician_headeronly
)

add_library(
  barelymusician_engine_performer
  performer.cpp
  performer.h
)
target_link_libraries(
  barelymusician_engine_performer
  barelymusician_engine_config
  barelymusician_engine_event
  barelymusician_engine_pool
  barelymusician_headeronly
)

add_library(
  barelymusician_engine_pool INTERFACE
  pool.h
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_engine_event_test
    event_test.cpp
  )
  target_link_libraries(
    barelymusician_engine_event_test
    barelymusician_engine_event
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barelymusician_engine_event_test)

  add_executable(
    barelymusician_engine_instrument_test
    instrument_test.cpp
  )
  target_link_libraries(
    barelymusician_engine_instrument_test
    barelymusician_engine_instrument
    barelymusician_dsp_decibels
    barelymusician_dsp_sample_data
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_engine_instrument_test)

  add_executable(
    barelymusician_engine_message_queue_test
    message_queue_test.cpp
  )
  target_link_libraries(
    barelymusician_engine_message_queue_test
    barelymusician_engine_message
    barelymusician_engine_message_queue
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_engine_message_queue_test)

  add_executable(
    barelymusician_engine_musician_test
    musician_test.cpp
  )
  target_link_libraries(
    barelymusician_engine_musician_test
    barelymusician_engine_instrument
    barelymusician_engine_musician
    barelymusician_engine_performer
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_engine_musician_test)

  add_executable(
    barelymusician_engine_performer_test
    performer_test.cpp
  )
  target_link_libraries(
    barelymusician_engine_performer_test
    barelymusician_engine_performer
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_engine_performer_test)
endif()
