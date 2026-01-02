target_sources(
  barelymusician PRIVATE
  instrument_params.h
  instrument_processor.h
  message.h
  message_queue.cpp
  message_queue.h
  note_params.h
  task_state.h
  voice_params.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    message_queue_test.cpp
  )
endif()
