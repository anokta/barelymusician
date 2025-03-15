target_sources(
  barelymusician PRIVATE
  callback.h
  find_or_null.h
  rng.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    find_or_null_test.cpp
  )
endif()
