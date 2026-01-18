#ifndef BARELYMUSICIAN_CORE_RNG_H_
#define BARELYMUSICIAN_CORE_RNG_H_

#include <cassert>
#include <random>

namespace barely {

template <typename EngineType, typename RealType>
class Rng {
 public:
  [[nodiscard]] int GetSeed() const noexcept { return seed_; }

  void SetSeed(int seed) noexcept {
    assert(seed >= 0);
    seed_ = seed;
    engine_.seed(seed_);
  }

  // Generates a new random number with uniform distribution in the normalized range [0, 1).
  [[nodiscard]] RealType Generate() noexcept { return distribution_(engine_); }

  // Generates a new random number with uniform distribution in the range [min, max).
  [[nodiscard]] uint32_t Generate(uint32_t min, uint32_t max) noexcept {
    assert(min <= max);
    return min + static_cast<uint32_t>(Generate() * static_cast<RealType>(max - min));
  }

 private:
  // Normalized uniform distribution in the range [0, 1).
  std::uniform_real_distribution<RealType> distribution_;

  // Random number generator engine.
  EngineType engine_;

  // Random number generator seed value.
  int seed_ = static_cast<int>(EngineType::default_seed);
};

// Random nubmer generator for the audio thread.
using AudioRng = Rng<std::minstd_rand, float>;

// Random nubmer generator for the main thread.
using MainRng = Rng<std::mt19937_64, double>;

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_RNG_H_
