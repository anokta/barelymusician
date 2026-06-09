barelymusician_target_sources(
  cmd.h
  cmd_queue.h
  engine_controller.h
  engine_processor.h
  engine_state.h
  instrument_controller.h
  instrument_processor.cpp
  instrument_processor.h
  params.h
  performer_controller.cpp
  performer_controller.h
  performer_state.h
  slice_pool.h
  voice_state.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    cmd_queue_test.cpp
    engine_controller_test.cpp
    engine_processor_test.cpp
    performer_controller_test.cpp
    slice_pool_test.cpp
  )
endif()
