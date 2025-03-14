target_sources(
  barelymusician PRIVATE
  context_free_grammar.h
)

if(ENABLE_TESTS)
  target_sources(
    barelymusician_test PRIVATE
    context_free_grammar_test.cpp
  )
endif()
