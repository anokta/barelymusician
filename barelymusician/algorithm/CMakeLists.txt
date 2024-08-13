add_library(
  barely_context_free_grammar INTERFACE
  context_free_grammar.h
)
target_link_libraries(
  barely_context_free_grammar INTERFACE
  barely_find_or_null
  barely_random
)

if(ENABLE_TESTS)
  add_executable(
    barely_context_free_grammar_test
    context_free_grammar_test.cpp
  )
  target_link_libraries(
    barely_context_free_grammar_test
    barely_context_free_grammar
    barely_random
    gmock
    gtest_main
  )
  gtest_discover_tests(barely_context_free_grammar_test)
endif()
