target_sources(
  barelymusician PRIVATE
  arp_state.h
  effect_params.h
  engine_processor.h
  instrument_controller.cpp
  instrument_controller.h
  instrument_params.h
  instrument_processor.cpp
  instrument_processor.h
  instrument_state.h
  message.h
  message_queue.h
  note_params.h
  performer_controller.cpp
  performer_controller.h
  performer_state.cpp
  performer_state.h
  task_state.h
  voice_params.h
  voice_state.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    # TODO(#126): clean up and reenable
    # instrument_test.cpp
    # performer_test.cpp
    message_queue_test.cpp
  )
endif()
