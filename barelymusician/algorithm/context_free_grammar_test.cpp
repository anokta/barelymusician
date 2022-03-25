#include "barelymusician/algorithm/context_free_grammar.h"

#include <string>
#include <vector>

#include "barelymusician/common/random.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace barelyapi {
namespace {

using ::testing::ElementsAre;

// Tests that the expected sequence is generated with a given substitution rule.
TEST(ContextFreeGrammarTest, GenerateSequence) {
  const std::string kStartSymbol = "Start";
  const std::vector<std::string> kSubstition = {"Intro", "Chorus", "Outro"};

  ContextFreeGrammar<std::string> grammar;
  grammar.AddRule(kStartSymbol, {kSubstition});

  Random random;
  const auto sequence = grammar.GenerateSequence(kStartSymbol, random);
  EXPECT_EQ(sequence, kSubstition);
}

// Tests that the expected sequence is generated with a given set of nested
// substitution rules.
TEST(ContextFreeGrammarTest, GenerateSequenceNestedRules) {
  ContextFreeGrammar<std::string> grammar;
  grammar.AddRule("Start", {{"Intro", "Body", "Outro"}});
  grammar.AddRule("Body", {{"Verse", "Chorus", "Bridge"}});
  grammar.AddRule("Bridge", {{"Break", "Chorus"}});

  Random random;
  const auto sequence = grammar.GenerateSequence("Start", random);
  EXPECT_THAT(sequence, ElementsAre("Intro", "Verse", "Chorus", "Break",
                                    "Chorus", "Outro"));
}

// Tests that the context free grammar generation always returns a sequence with
// the expected size range when rules with multiple substitutions are present.
TEST(ContextFreeGrammarTest, GenerateSequenceExpectedSizeRange) {
  const int kNumGenerations = 10;
  const int kStartSymbol = 0;
  const int kEndSymbol = -1;
  const int kMinSize = 2;
  const int kMaxSize = 5;

  ContextFreeGrammar<int> grammar;
  std::vector<std::vector<int>> substitutions;
  for (int i = kMinSize; i <= kMaxSize; ++i) {
    substitutions.push_back({i});
  }
  grammar.AddRule(kStartSymbol, substitutions);
  for (int i = kMinSize; i <= kMaxSize; ++i) {
    grammar.AddRule(i, {std::vector<int>(i, kEndSymbol)});
  }

  Random random;
  for (int n = 0; n < kNumGenerations; ++n) {
    const auto sequence = grammar.GenerateSequence(kStartSymbol, random);
    EXPECT_GE(sequence.size(), kMinSize);
    EXPECT_LE(sequence.size(), kMaxSize);
  }
}

// Tests that no substitutions are done when there are no rules given.
TEST(ContextFreeGrammarTest, GenerateSequenceNoRules) {
  const int kStartSymbol = 0;

  ContextFreeGrammar<int> grammar;

  Random random;
  const auto sequence = grammar.GenerateSequence(kStartSymbol, random);
  EXPECT_THAT(sequence, ElementsAre(kStartSymbol));
}

// Tests that no substitutions are done when there are no rules given for the
// start symbol.
TEST(ContextFreeGrammarTest, GenerateSequenceNoStartSymbolRule) {
  const int kStartSymbol = 3;

  ContextFreeGrammar<int> grammar;
  grammar.AddRule(0, {{10, 11, 12}});
  grammar.AddRule(1, {{13}, {14}});
  grammar.AddRule(2, {{15}});

  Random random;
  const auto sequence = grammar.GenerateSequence(kStartSymbol, random);
  EXPECT_THAT(sequence, ElementsAre(kStartSymbol));
}

}  // namespace
}  // namespace barelyapi
