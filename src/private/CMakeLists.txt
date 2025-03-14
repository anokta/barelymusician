target_sources(
  barelymusician PRIVATE
  arpeggiator_impl.cpp
  arpeggiator_impl.h
  engine_impl.cpp
  engine_impl.h
  instrument_impl.cpp
  instrument_impl.h
  performer_impl.cpp
  performer_impl.h
  random_impl.cpp
  random_impl.h
  repeater_impl.cpp
  repeater_impl.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    engine_impl_test.cpp
    instrument_impl_test.cpp
    performer_impl_test.cpp
    random_impl_test.cpp
  )
endif()
