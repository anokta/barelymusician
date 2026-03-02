target_sources(
  barelymusician PRIVATE
  bit_crusher.h
  compressor.h
  delay_filter.h
  distortion.h
  envelope.h
  tone_filter.h
  one_pole_filter.h
  reverb.h
  sample_generators.h
  sidechain.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    bit_crusher_test.cpp
    distortion_test.cpp
    envelope_test.cpp
    one_pole_filter_test.cpp
    sample_generators_test.cpp
  )
endif()
