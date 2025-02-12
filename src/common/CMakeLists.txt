add_library(
  barelymusician_common_callback INTERFACE
  callback.h
)

add_library(
  barelymusician_common_find_or_null INTERFACE
  find_or_null.h
)

add_library(
  barelymusician_common_message INTERFACE
  message.h
)
target_link_libraries(
  barelymusician_common_message INTERFACE
  barelymusician_dsp_sample_data
  barelymusician_headeronly
)

add_library(
  barelymusician_common_message_queue
  message_queue.cpp
  message_queue.h
)
target_link_libraries(
  barelymusician_common_message_queue
  barelymusician_common_message
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_common_find_or_null_test
    find_or_null_test.cpp
  )
  target_link_libraries(
    barelymusician_common_find_or_null_test
    barelymusician_common_find_or_null
    gtest_main
  )
  gtest_discover_tests(barelymusician_common_find_or_null_test)

  add_executable(
    barelymusician_common_message_queue_test
    message_queue_test.cpp
  )
  target_link_libraries(
    barelymusician_common_message_queue_test
    barelymusician_common_message
    barelymusician_common_message_queue
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_common_message_queue_test)
endif()
