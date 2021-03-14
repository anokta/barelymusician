#ifndef BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
#define BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_

#include <iterator>
#include <unordered_map>
#include <vector>

#include "barelymusician/common/logging.h"
#include "barelymusician/common/random_generator.h"

namespace barelyapi {

/// Context-free grammar (L-system) template that generates a sequence of
/// |SymbolType| with the given set of substition rules.
template <typename SymbolType>
class ContextFreeGrammar {
 public:
  /// Constructs new |ContextFreeGrammar|.
  ///
  /// @param random_generator Pointer to random number generator.
  explicit ContextFreeGrammar(RandomGenerator* random_generator);

  /// Adds a rule of |substitions| for the given |symbol|.
  ///
  /// @param symbol Input symbol.
  /// @param substitions Output substition list of symbols.
  void AddRule(const SymbolType& symbol,
               const std::vector<std::vector<SymbolType>>& substitions);

  /// Generates a new sequence beginning from the given |start_symbol|.
  ///
  /// @param start_symbol Initial symbol to start the sequence.
  /// @return Generated symbol sequence.
  std::vector<SymbolType> GenerateSequence(
      const SymbolType& start_symbol) const;

 private:
  // Returns new substition for the given |symbol| using its rule.
  std::vector<SymbolType> GetSubstition(const SymbolType& symbol) const;

  // Random number generator.
  RandomGenerator* random_generator_;  // Not owned.

  // Grammar rules that map symbols to their corresponding substitions.
  std::unordered_map<SymbolType, std::vector<std::vector<SymbolType>>> rules_;
};

template <typename SymbolType>
ContextFreeGrammar<SymbolType>::ContextFreeGrammar(
    RandomGenerator* random_generator)
    : random_generator_(random_generator) {
  DCHECK(random_generator);
}

template <typename SymbolType>
void ContextFreeGrammar<SymbolType>::AddRule(
    const SymbolType& symbol,
    const std::vector<std::vector<SymbolType>>& substitions) {
  rules_[symbol] = substitions;
}

template <typename SymbolType>
std::vector<SymbolType> ContextFreeGrammar<SymbolType>::GenerateSequence(
    const SymbolType& start_symbol) const {
  std::vector<SymbolType> sequence;

  // Add |start_symbol| to the beginning of the sequence.
  sequence.push_back(start_symbol);
  // Iterate through all the symbols, and substitute them according to their
  // corresponding rules until reaching to the end.
  int i = 0;
  while (i < static_cast<int>(sequence.size())) {
    if (rules_.find(sequence[i]) == rules_.cend()) {
      ++i;
      continue;
    }
    const auto& substition = GetSubstition(sequence[i]);
    sequence.erase(std::next(sequence.cbegin(), i));
    sequence.insert(std::next(sequence.cbegin(), i), substition.cbegin(),
                    substition.cend());
  }

  return sequence;
}

template <typename SymbolType>
std::vector<SymbolType> ContextFreeGrammar<SymbolType>::GetSubstition(
    const SymbolType& symbol) const {
  if (rules_.find(symbol) == rules_.cend() || rules_.at(symbol).size() == 0) {
    DLOG(INFO) << "Substition rule does not exist for symbol: " << symbol;
    return std::vector<SymbolType>();
  }

  // Select a substition randomly with equal probability for each selection.
  const auto& substitions = rules_.at(symbol);
  const int index = random_generator_->DrawUniform(
      0, static_cast<int>(substitions.size()) - 1);
  return substitions[index];
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
