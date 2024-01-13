add_library(
  barely_find_or_null INTERFACE
  find_or_null.h
)

add_library(
  barely_random
  random.cpp
  random.h
)

add_library(
  barely_internal_rational
  rational.cpp
  rational.h
)
target_link_libraries(
  barely_internal_rational
  barelymusician_headeronly
)

add_library(
  barely_seconds
  seconds.cpp
  seconds.h
)
target_link_libraries(
  barely_seconds
  barely_rational
  barelymusician_headeronly
)

if (ENABLE_TESTS)
  add_executable(
    barely_find_or_null_test
    find_or_null_test.cpp
  )
  target_link_libraries(
    barely_find_or_null_test
    barely_find_or_null
    gtest_main
  )
  gtest_discover_tests(barely_find_or_null_test)

  add_executable(
    barely_random_test
    random_test.cpp
  )
  target_link_libraries(
    barely_random_test
    barely_random
    gtest_main
  )
  gtest_discover_tests(barely_random_test)

  add_executable(
    barely_rational_test
    rational_test.cpp
  )
  target_link_libraries(
    barely_rational_test
    barely_rational
    barely_internal_rational
    gtest_main
  )
  gtest_discover_tests(barely_rational_test)

  add_executable(
    barely_seconds_test
    seconds_test.cpp
  )
  target_link_libraries(
    barely_seconds_test
    barely_rational
    barely_seconds
    barelymusician_headeronly
    gtest_main
  )
  gtest_discover_tests(barely_seconds_test)
endif()
