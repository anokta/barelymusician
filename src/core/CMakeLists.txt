barelymusician_target_sources(
  arena.h
  callback.h
  constants.h
  control.h
  decibels.h
  pool.h
  rng.h
  scale.h
  time.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    decibels_test.cpp
    pool_test.cpp
    scale_test.cpp
    time_test.cpp
  )
endif()
