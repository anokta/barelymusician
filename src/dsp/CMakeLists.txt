target_sources(
  barelymusician PRIVATE
  biquad_filter.h
  bit_crusher.h
  compressor.h
  decibels.h
  delay_filter.h
  distortion.h
  envelope.h
  one_pole_filter.h
  sample_generators.h
  sidechain.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    biquad_filter_test.cpp
    bit_crusher_test.cpp
    decibels_test.cpp
    distortion_test.cpp
    envelope_test.cpp
    one_pole_filter_test.cpp
    sample_generators_test.cpp
  )
endif()
