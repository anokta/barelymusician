add_library(
  context_free_grammar INTERFACE
  context_free_grammar.h
)
target_link_libraries(
  context_free_grammar INTERFACE
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
gtest_discover_tests(context_free_grammar_test)
