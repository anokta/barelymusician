add_library(
  context_free_grammar INTERFACE
)
target_link_libraries(
  context_free_grammar INTERFACE
  find_or_null
  random
)

add_executable(
  context_free_grammar_test
  context_free_grammar_test.cpp
)
target_link_libraries(
  context_free_grammar_test
  context_free_grammar
  random
  gmock
  gtest_main
)
include(GoogleTest)
gtest_discover_tests(context_free_grammar_test)
