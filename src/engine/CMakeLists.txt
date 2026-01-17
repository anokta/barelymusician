target_sources(
  barelymusician PRIVATE
  arp_state.h
  effect_params.h
  engine_controller.h
  engine_processor.h
  engine_state.h
  instrument_controller.cpp
  instrument_controller.h
  instrument_params.h
  instrument_processor.cpp
  instrument_processor.h
  instrument_state.h
  message.h
  message_queue.h
  note_params.h
  note_state.h
  performer_controller.cpp
  performer_controller.h
  performer_state.h
  slice_pool.h
  slice_state.h
  task_state.h
  voice_params.h
  voice_state.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    engine_controller_test.cpp
    engine_processor_test.cpp
    instrument_controller_test.cpp
    message_queue_test.cpp
    performer_controller_test.cpp
    slice_pool_test.cpp
  )
endif()
