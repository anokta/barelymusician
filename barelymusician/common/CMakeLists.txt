add_library(
  find_or_null INTERFACE
  find_or_null.h
)

add_library(
  random
  random.cpp
  random.h
)

add_executable(
  find_or_null_test
  find_or_null_test.cpp
)
target_link_libraries(
  find_or_null_test
  find_or_null
  gtest_main
)
gtest_discover_tests(find_or_null_test)

add_executable(
  random_test
  random_test.cpp
)
target_link_libraries(
  random_test
  random
  gtest_main
)
gtest_discover_tests(random_test)
