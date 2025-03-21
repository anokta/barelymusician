#include "algorithm/context_free_grammar.h"

#include <string>
#include <vector>

#include "common/rng.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace barely {
namespace {

using ::testing::ElementsAre;

// Tests that the expected sequence is generated with a given substitution rule.
TEST(ContextFreeGrammarTest, GenerateSequence) {
  const std::string kStartSymbol = "Start";
  const std::vector<std::string> kSubstition = {"Intro", "Chorus", "Outro"};

  ContextFreeGrammar<std::string> grammar;
  grammar.AddRule(kStartSymbol, {kSubstition});

  MainRng rng;
  const auto sequence = grammar.GenerateSequence(kStartSymbol, rng);
  EXPECT_EQ(sequence, kSubstition);
}

// Tests that the expected sequence is generated with a given set of nested substitution rules.
TEST(ContextFreeGrammarTest, GenerateSequenceNestedRules) {
  ContextFreeGrammar<std::string> grammar;
  grammar.AddRule("Start", {{"Intro", "Body", "Outro"}});
  grammar.AddRule("Body", {{"Verse", "Chorus", "Bridge"}});
  grammar.AddRule("Bridge", {{"Break", "Chorus"}});

  MainRng rng;
  const auto sequence = grammar.GenerateSequence("Start", rng);
  EXPECT_THAT(sequence, ElementsAre("Intro", "Verse", "Chorus", "Break", "Chorus", "Outro"));
}

// Tests that the context free grammar generation always returns a sequence with the expected size
// range when rules with multiple substitutions are present.
TEST(ContextFreeGrammarTest, GenerateSequenceExpectedSizeRange) {
  constexpr int kGenerationCount = 10;
  constexpr int kStartSymbol = 0;
  constexpr int kEndSymbol = -1;
  constexpr int kMinSize = 2;
  constexpr int kMaxSize = 5;

  ContextFreeGrammar<int> grammar;
  std::vector<std::vector<int>> substitutions;
  for (int i = kMinSize; i <= kMaxSize; ++i) {
    substitutions.push_back({i});
  }
  grammar.AddRule(kStartSymbol, substitutions);
  for (int i = kMinSize; i <= kMaxSize; ++i) {
    grammar.AddRule(i, {std::vector<int>(i, kEndSymbol)});
  }

  MainRng rng;
  for (int i = 0; i < kGenerationCount; ++i) {
    const auto sequence = grammar.GenerateSequence(kStartSymbol, rng);
    EXPECT_GE(sequence.size(), kMinSize);
    EXPECT_LE(sequence.size(), kMaxSize);
  }
}

// Tests that no substitutions are done when there are no rules given.
TEST(ContextFreeGrammarTest, GenerateSequenceNoRules) {
  constexpr int kStartSymbol = 0;

  const ContextFreeGrammar<int> grammar;

  MainRng rng;
  const auto sequence = grammar.GenerateSequence(kStartSymbol, rng);
  EXPECT_THAT(sequence, ElementsAre(kStartSymbol));
}

// Tests that no substitutions are done when there are no rules given for the start symbol.
TEST(ContextFreeGrammarTest, GenerateSequenceNoStartSymbolRule) {
  constexpr int kStartSymbol = 3;

  ContextFreeGrammar<int> grammar;
  grammar.AddRule(0, {{10, 11, 12}});
  grammar.AddRule(1, {{13}, {14}});
  grammar.AddRule(2, {{15}});

  MainRng rng;
  const auto sequence = grammar.GenerateSequence(kStartSymbol, rng);
  EXPECT_THAT(sequence, ElementsAre(kStartSymbol));
}

}  // namespace
}  // namespace barely
