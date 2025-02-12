add_library(
  barelymusician_internal_callback INTERFACE
  callback.h
)

add_library(
  barelymusician_internal_engine_impl
  engine_impl.cpp
  engine_impl.h
)
target_link_libraries(
  barelymusician_internal_engine_impl
  barelymusician_internal_instrument_impl
  barelymusician_internal_performer_impl
  barelymusician_headeronly
)

add_library(
  barelymusician_internal_instrument_impl
  instrument_impl.cpp
  instrument_impl.h
)
target_link_libraries(
  barelymusician_internal_instrument_impl
  barelymusician_internal_callback
  barelymusician_internal_message
  barelymusician_internal_message_queue
  barelymusician_common_find_or_null
  barelymusician_dsp_decibels
  barelymusician_dsp_instrument_processor
  barelymusician_dsp_one_pole_filter
  barelymusician_dsp_oscillator
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
  barelymusician_internal_performer_impl
  performer_impl.cpp
  performer_impl.h
)
target_link_libraries(
  barelymusician_internal_performer_impl
  barelymusician_internal_callback
  barelymusician_headeronly
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_internal_engine_impl_test
    engine_impl_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_engine_impl_test
    barelymusician_internal_engine_impl
    barelymusician_internal_instrument_impl
    barelymusician_internal_performer_impl
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_engine_impl_test)

  add_executable(
    barelymusician_internal_instrument_impl_test
    instrument_impl_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_instrument_impl_test
    barelymusician_internal_instrument_impl
    barelymusician_dsp_decibels
    barelymusician_dsp_sample_data
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_instrument_impl_test)

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
    barelymusician_internal_performer_impl_test
    performer_impl_test.cpp
  )
  target_link_libraries(
    barelymusician_internal_performer_impl_test
    barelymusician_internal_performer_impl
    barelymusician_headeronly
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_internal_performer_impl_test)
endif()
