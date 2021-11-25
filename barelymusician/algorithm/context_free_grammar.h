#ifndef BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
#define BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_

#include <cassert>
#include <iterator>
#include <unordered_map>
#include <vector>

#include "barelymusician/common/find_or_null.h"
#include "barelymusician/common/random.h"

namespace barely {

/// Context-free grammar (L-system) template that generates a sequence of
/// |SymbolType| with the given set of substition rules.
template <typename SymbolType>
class ContextFreeGrammar {
 public:
  /// Constructs new |ContextFreeGrammar|.
  ///
  /// @param random Pointer to random number generator.
  explicit ContextFreeGrammar(Random* random);

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
  // Returns substition for a given |symbol| using its rule.
  const std::vector<SymbolType>* GetSubstition(const SymbolType& symbol) const;

  // Random number generator.
  Random& random_;

  // Grammar rules that map symbols to their corresponding substitions.
  std::unordered_map<SymbolType, std::vector<std::vector<SymbolType>>> rules_;
};

template <typename SymbolType>
ContextFreeGrammar<SymbolType>::ContextFreeGrammar(Random* random)
    : random_(*random) {
  assert(random);
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
    if (rules_.find(sequence[i]) != rules_.cend()) {
      const auto* substition = GetSubstition(sequence[i]);
      sequence.erase(std::next(sequence.cbegin(), i));
      if (substition) {
        sequence.insert(std::next(sequence.cbegin(), i), substition->cbegin(),
                        substition->cend());
      }
    } else {
      ++i;
    }
  }
  return sequence;
}

template <typename SymbolType>
const std::vector<SymbolType>* ContextFreeGrammar<SymbolType>::GetSubstition(
    const SymbolType& symbol) const {
  const auto* substitions = FindOrNull(rules_, symbol);
  if (!substitions || substitions->empty()) {
    return nullptr;
  }
  // Select a substition randomly with equal probability for each selection.
  const int index =
      random_.DrawUniform(0, static_cast<int>(substitions->size()) - 1);
  return &(*substitions)[index];
}

}  // namespace barely

#endif  // BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
