add_library(
  barelymusician_algorithm_context_free_grammar INTERFACE
  context_free_grammar.h
)
target_link_libraries(
  barelymusician_algorithm_context_free_grammar INTERFACE
  barelymusician_common_find_or_null
  barelymusician_common_random
)

if(ENABLE_TESTS)
  add_executable(
    barelymusician_algorithm_context_free_grammar_test
    context_free_grammar_test.cpp
  )
  target_link_libraries(
    barelymusician_algorithm_context_free_grammar_test
    barelymusician_algorithm_context_free_grammar
    barelymusician_common_random
    gmock
    gtest_main
  )
  gtest_discover_tests(barelymusician_algorithm_context_free_grammar_test)
endif()
