add_library(
  barelymusician_common_find_or_null INTERFACE
  find_or_null.h
)

add_library(
  barelymusician_common_random_impl
  random_impl.cpp
  random_impl.h
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_common_find_or_null_test
    find_or_null_test.cpp
  )
  target_link_libraries(
    barelymusician_common_find_or_null_test
    barelymusician_common_find_or_null
    gtest_main
  )
  gtest_discover_tests(barelymusician_common_find_or_null_test)

  add_executable(
    barelymusician_common_random_impl_test
    random_impl_test.cpp
  )
  target_link_libraries(
    barelymusician_common_random_impl_test
    barelymusician_common_random_impl
    gtest_main
  )
  gtest_discover_tests(barelymusician_common_random_impl_test)
endif()
