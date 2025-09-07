target_sources(
  barelymusician PRIVATE
  biquad_filter.h
  bit_crusher.h
  control.h
  decibels.h
  delay_filter.h
  engine_processor.cpp
  engine_processor.h
  envelope.h
  instrument_processor.cpp
  instrument_processor.h
  message.h
  message_queue.cpp
  message_queue.h
  sample_data.cpp
  sample_data.h
  sample_generators.h
  voice.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    biquad_filter_test.cpp
    bit_crusher_test.cpp
    decibels_test.cpp
    envelope_test.cpp
    instrument_processor_test.cpp
    message_queue_test.cpp
    sample_data_test.cpp
    sample_generators_test.cpp
  )
endif()
