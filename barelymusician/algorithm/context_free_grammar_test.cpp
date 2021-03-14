#include "barelymusician/algorithm/context_free_grammar.h"

#include <string>
#include <vector>

#include "barelymusician/common/random_generator.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the expected sequence is generated with a given substition rule.
TEST(ContextFreeGrammarTest, GenerateSequence) {
  const std::string kStartSymbol = "Start";
  const std::vector<std::string> kSubstition = {"Intro", "Chorus", "Outro"};

  RandomGenerator random_generator;
  ContextFreeGrammar<std::string> grammar(&random_generator);
  grammar.AddRule(kStartSymbol, {kSubstition});

  const auto sequence = grammar.GenerateSequence(kStartSymbol);
  EXPECT_EQ(sequence, kSubstition);
}

// Tests that the expected sequence is generated with a given set of nested
// substition rules.
TEST(ContextFreeGrammarTest, GenerateSequenceNestedRules) {
  RandomGenerator random_generator;
  ContextFreeGrammar<std::string> grammar(&random_generator);
  grammar.AddRule("Start", {{"Intro", "Body", "Outro"}});
  grammar.AddRule("Body", {{"Verse", "Chorus", "Bridge"}});
  grammar.AddRule("Bridge", {{"Break", "Chorus"}});

  const std::vector<std::string> kExpectedSequence = {
      "Intro", "Verse", "Chorus", "Break", "Chorus", "Outro"};
  const auto sequence = grammar.GenerateSequence("Start");
  EXPECT_EQ(sequence, kExpectedSequence);
}

// Tests that the context free grammar generation always returns a sequence with
// the expected size range when rules with multiple substitions are present.
TEST(ContextFreeGrammarTest, GenerateSequenceExpectedSizeRange) {
  const int kNumGenerations = 10;
  const int kStartSymbol = 0;
  const int kEndSymbol = -1;
  const int kMinSize = 2;
  const int kMaxSize = 5;

  RandomGenerator random_generator;
  ContextFreeGrammar<int> grammar(&random_generator);
  std::vector<std::vector<int>> substitions;
  for (int i = kMinSize; i <= kMaxSize; ++i) {
    substitions.push_back({i});
  }
  grammar.AddRule(kStartSymbol, substitions);
  for (int i = kMinSize; i <= kMaxSize; ++i) {
    grammar.AddRule(i, {std::vector<int>(i, kEndSymbol)});
  }

  for (int n = 0; n < kNumGenerations; ++n) {
    const auto sequence = grammar.GenerateSequence(kStartSymbol);
    EXPECT_GE(sequence.size(), kMinSize);
    EXPECT_LE(sequence.size(), kMaxSize);
  }
}

// Tests that no substitions are done when there are no rules given.
TEST(ContextFreeGrammarTest, GenerateSequenceNoRules) {
  const int kStartSymbol = 0;

  RandomGenerator random_generator;
  ContextFreeGrammar<int> grammar(&random_generator);

  const auto sequence = grammar.GenerateSequence(kStartSymbol);
  ASSERT_EQ(sequence.size(), 1);
  EXPECT_EQ(sequence.front(), kStartSymbol);
}

// Tests that no substitions are done when there are no rules given for the
// start symbol.
TEST(ContextFreeGrammarTest, GenerateSequenceNoStartSymbolRule) {
  const int kStartSymbol = 3;

  RandomGenerator random_generator;
  ContextFreeGrammar<int> grammar(&random_generator);
  grammar.AddRule(0, {{10, 11, 12}});
  grammar.AddRule(1, {{13}, {14}});
  grammar.AddRule(2, {{15}});

  const auto sequence = grammar.GenerateSequence(kStartSymbol);
  ASSERT_EQ(sequence.size(), 1);
  EXPECT_EQ(sequence.front(), kStartSymbol);
}

}  // namespace
}  // namespace barelyapi
