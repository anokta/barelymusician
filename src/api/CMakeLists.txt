target_sources(
  barelymusician PRIVATE
  arpeggiator.cpp
  arpeggiator.h
  engine.cpp
  engine.h
  instrument.cpp
  instrument.h
  performer.cpp
  performer.h
  repeater.cpp
  repeater.h
  task.cpp
  task.h
  trigger.cpp
  trigger.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    engine_test.cpp
    instrument_test.cpp
    performer_test.cpp
  )
endif()
