target_sources(
  barelymusician PRIVATE
  callback.h
  constants.h
  control.h
  pool.h
  rng.h
  time.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    pool_test.cpp
    time_test.cpp
  )
endif()
