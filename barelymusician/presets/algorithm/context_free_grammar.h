#ifndef BARELYMUSICIAN_PRESETS_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
#define BARELYMUSICIAN_PRESETS_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_

#include <iterator>
#include <unordered_map>
#include <utility>
#include <vector>

#include "examples/common/random.h"

namespace barelyapi {

/// Context-free grammar (L-system) template that generates a sequence of
/// `SymbolType` with the given set of substitution rules.
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
  /// @param random Random number generator.
  /// @return Generated symbol sequence.
  std::vector<SymbolType> GenerateSequence(const SymbolType& start_symbol,
                                           Random& random) const noexcept;

 private:
  // Returns a substitution for a given `symbol` with a `random` draw.
  const std::vector<SymbolType>* GetSubstitution(const SymbolType& symbol,
                                                 Random& random) const noexcept;

  // Grammar rules that map symbols to their corresponding substitutions.
  std::unordered_map<SymbolType, std::vector<std::vector<SymbolType>>> rules_;
};

template <typename SymbolType>
void ContextFreeGrammar<SymbolType>::AddRule(
    const SymbolType& symbol,
    std::vector<std::vector<SymbolType>> substitutions) noexcept {
  rules_[symbol] = std::move(substitutions);
}

template <typename SymbolType>
std::vector<SymbolType> ContextFreeGrammar<SymbolType>::GenerateSequence(
    const SymbolType& start_symbol, Random& random) const noexcept {
  std::vector<SymbolType> sequence;
  // Add `start_symbol` to the beginning of the sequence.
  sequence.push_back(start_symbol);
  // Iterate through all the symbols, and substitute them according to their
  // corresponding rules until reaching to the end.
  int i = 0;
  while (i < static_cast<int>(sequence.size())) {
    if (rules_.find(sequence[i]) != rules_.cend()) {
      const auto* substitution = GetSubstitution(sequence[i], random);
      sequence.erase(std::next(sequence.begin(), i));
      if (substitution) {
        sequence.insert(std::next(sequence.begin(), i), substitution->cbegin(),
                        substitution->cend());
      }
    } else {
      ++i;
    }
  }
  return sequence;
}

template <typename SymbolType>
const std::vector<SymbolType>* ContextFreeGrammar<SymbolType>::GetSubstitution(
    const SymbolType& symbol, Random& random) const noexcept {
  const auto it = rules_.find(symbol);
  if (it == rules_.cend() || it->second.empty()) {
    return nullptr;
  }
  // Select a substitution randomly with equal probability for each selection.
  const auto& substitutions = it->second;
  const int index =
      random.DrawUniform(0, static_cast<int>(substitutions.size()) - 1);
  return &substitutions[index];
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_PRESETS_ALGORITHM_CONTEXT_FREE_GRAMMAR_H_
