target_sources(
  barelymusician PRIVATE
  engine.cpp
  engine.h
  performer.cpp
  performer.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    engine_test.cpp
    performer_test.cpp
  )
endif()
