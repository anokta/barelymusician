target_sources(
  barelymusician PRIVATE
  engine.cpp
  engine.h
  instrument.cpp
  instrument.h
  performer.cpp
  performer.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    engine_test.cpp
    instrument_test.cpp
    performer_test.cpp
  )
endif()
