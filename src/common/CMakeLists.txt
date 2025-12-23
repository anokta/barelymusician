target_sources(
  barelymusician PRIVATE
  callback.h
  constants.h
  find_or_null.h
  rng.h
  time.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    find_or_null_test.cpp
    time_test.cpp
  )
endif()
