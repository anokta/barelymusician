target_sources(
  barelymusician PRIVATE
  message.h
  message_queue.cpp
  message_queue.h
  task_state.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    message_queue_test.cpp
  )
endif()
