target_sources(
  barelymusician PRIVATE
  callback.h
  find_or_null.h
  mutable.h
  rng.h
  time.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    find_or_null_test.cpp
    mutable_test.cpp
    time_test.cpp
  )
endif()
