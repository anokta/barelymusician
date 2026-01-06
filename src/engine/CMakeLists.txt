target_sources(
  barelymusician PRIVATE
  arp_state.h
  instrument_controller.cpp
  instrument_controller.h
  instrument_params.h
  instrument_processor.h
  instrument_state.h
  message.h
  message_queue.h
  note_params.h
  task_state.h
  voice_params.h
  voice_processor.h
  voice_state.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    # TODO(#126): move this to instrument_controller_test
    # instrument_test.cpp
    message_queue_test.cpp
  )
endif()
