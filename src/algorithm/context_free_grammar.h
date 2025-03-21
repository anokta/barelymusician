
#ifndef BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
#define BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_

#include <iterator>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/find_or_null.h"
#include "common/rng.h"

namespace barely {

/// Context-free grammar (L-system) template that generates a sequence of `SymbolType` with the
/// given set of substitution rules.
template <typename SymbolType>
class ContextFreeGrammar {
 public:
  /// Adds a rule of `substitutions` for the given `symbol`.
  ///
  /// @param symbol Input symbol.
  /// @param substitutions Output list of substitution symbols.
  void AddRule(const SymbolType& symbol,
               std::vector<std::vector<SymbolType>> substitutions) noexcept;

  /// Generates a new sequence beginning from the given `start_symbol`.
  ///
  /// @param start_symbol Initial symbol to start the sequence.
  /// @param rng Random number generator.
  /// @return Generated symbol sequence.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  std::vector<SymbolType> GenerateSequence(const SymbolType& start_symbol,
                                           MainRng& rng) const noexcept;

 private:
  // Returns a substitution from a given `substitutions` with a random draw.
  const std::vector<SymbolType>* GetSubstitution(
      const std::vector<std::vector<SymbolType>>& substitutions, MainRng& rng) const noexcept;

  // Grammar rules that map symbols to their corresponding substitutions.
  std::unordered_map<SymbolType, std::vector<std::vector<SymbolType>>> rules_;
};

template <typename SymbolType>
void ContextFreeGrammar<SymbolType>::AddRule(
    const SymbolType& symbol, std::vector<std::vector<SymbolType>> substitutions) noexcept {
  rules_[symbol] = std::move(substitutions);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
template <typename SymbolType>
std::vector<SymbolType> ContextFreeGrammar<SymbolType>::GenerateSequence(
    const SymbolType& start_symbol, MainRng& rng) const noexcept {
  std::vector<SymbolType> sequence;
  // Add `start_symbol` to the beginning of the sequence.
  sequence.push_back(start_symbol);
  // Iterate through all the symbols, and substitute them according to their corresponding rules
  // until reaching to the end.
  int i = 0;
  while (i < static_cast<int>(sequence.size())) {
    if (const auto* substitutions = FindOrNull(rules_, sequence[i])) {
      const auto* substitution = GetSubstitution(*substitutions, rng);
      sequence.erase(std::next(sequence.begin(), i));
      if (substitution) {
        sequence.insert(std::next(sequence.begin(), i), substitution->begin(), substitution->end());
      }
    } else {
      ++i;
    }
  }
  return sequence;
}

template <typename SymbolType>
const std::vector<SymbolType>* ContextFreeGrammar<SymbolType>::GetSubstitution(
    const std::vector<std::vector<SymbolType>>& substitutions, MainRng& rng) const noexcept {
  // Select a substitution randomly with equal probability for each selection.
  if (!substitutions.empty()) {
    const int index = rng.Generate(0, static_cast<int>(substitutions.size()));
    return &substitutions[index];
  }
  return nullptr;
}

}  // namespace barely

#endif  // BARELYMUSICIAN_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
