add_library(
  find_or_null INTERFACE
)

add_library(
  id INTERFACE
)

add_library(
  id_generator
  id_generator.cpp
)
target_link_libraries(
  id_generator
  id
)

add_library(
  random
  random.cpp
)

add_library(
  status INTERFACE
)

add_library(
  visitor INTERFACE
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
  id_generator_test
  id_generator_test.cpp
)
target_link_libraries(
  id_generator_test
  id_generator
  gtest_main
)
gtest_discover_tests(id_generator_test)

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

add_executable(
  visitor_test
  visitor_test.cpp
)
target_link_libraries(
  visitor_test
  visitor
  gtest_main
)
gtest_discover_tests(visitor_test)
