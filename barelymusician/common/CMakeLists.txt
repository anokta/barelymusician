add_library(
  barely_find_or_null INTERFACE
  find_or_null.h
)

add_library(
  barely_random
  random.cpp
  random.h
)

if(ENABLE_TESTS)
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
endif()
