#include "barelymusician/algorithm/context_free_grammar.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"

namespace barelyapi {
namespace {

// Tests that the expected sequence is generated with a given substition rule.
TEST(ContextFreeGrammarTest, GenerateSequence) {
  const std::string kStartSymbol = "Start";
  const std::vector<std::string> kSubstition = {"Intro", "Chorus", "Outro"};

  ContextFreeGrammar<std::string> grammar;
  grammar.AddRule(kStartSymbol, {kSubstition});

  const auto sequence = grammar.GenerateSequence(kStartSymbol);
  EXPECT_EQ(kSubstition, sequence);
}

// Tests that the expected sequence is generated with a given set of nested
// substition rules.
TEST(ContextFreeGrammarTest, GenerateSequenceNestedRules) {
  ContextFreeGrammar<std::string> grammar;
  grammar.AddRule("Start", {{"Intro", "Body", "Outro"}});
  grammar.AddRule("Body", {{"Verse", "Chorus", "Bridge"}});
  grammar.AddRule("Bridge", {{"Break", "Chorus"}});

  const std::vector<std::string> kExpectedSequence = {
      "Intro", "Verse", "Chorus", "Break", "Chorus", "Outro"};
  const auto sequence = grammar.GenerateSequence("Start");
  EXPECT_EQ(kExpectedSequence, sequence);
}

// Tests that the context free grammar generation always returns a sequence with
// the expected size range when rules with multiple substitions are present.
TEST(ContextFreeGrammarTest, GenerateSequenceExpectedSizeRange) {
  const int kNumGenerations = 10;
  const int kStartSymbol = 0;
  const int kEndSymbol = -1;
  const int kMinSize = 2;
  const int kMaxSize = 5;

  ContextFreeGrammar<int> grammar;
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

  ContextFreeGrammar<int> grammar;

  const auto sequence = grammar.GenerateSequence(kStartSymbol);
  ASSERT_EQ(1, sequence.size());
  EXPECT_EQ(kStartSymbol, sequence.front());
}

// Tests that no substitions are done when there are no rules given for the
// start symbol.
TEST(ContextFreeGrammarTest, GenerateSequenceNoStartSymbolRule) {
  const int kStartSymbol = 3;

  ContextFreeGrammar<int> grammar;
  grammar.AddRule(0, {{10, 11, 12}});
  grammar.AddRule(1, {{13}, {14}});
  grammar.AddRule(2, {{15}});

  const auto sequence = grammar.GenerateSequence(kStartSymbol);
  ASSERT_EQ(1, sequence.size());
  EXPECT_EQ(kStartSymbol, sequence.front());
}

}  // namespace
}  // namespace barelyapi
