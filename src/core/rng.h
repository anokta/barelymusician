#ifndef BARELYMUSICIAN_CORE_RNG_H_
#define BARELYMUSICIAN_CORE_RNG_H_

#include <cassert>
#include <climits>
#include <ctime>
#include <random>

namespace barely {

template <typename EngineType, typename RealType>
class Rng {
 public:
  Rng() noexcept
      : seed_(static_cast<int32_t>(static_cast<uint32_t>(std::time(nullptr)) & INT_MAX)),
        engine_(seed_) {}

  void ResetSeed(int32_t seed) noexcept {
    assert(seed >= 0);
    seed_ = seed;
    engine_.seed(seed_);
  }

  [[nodiscard]] RealType Generate() noexcept { return distribution_(engine_); }

  [[nodiscard]] uint32_t Generate(uint32_t max) noexcept {
    return static_cast<uint32_t>(Generate() * static_cast<RealType>(max));
  }

 private:
  std::uniform_real_distribution<RealType> distribution_;
  int32_t seed_ = 0;
  EngineType engine_;
};

using AudioRng = Rng<std::minstd_rand, float>;
using MainRng = Rng<std::mt19937_64, double>;

}  // namespace barely

#endif  // BARELYMUSICIAN_CORE_RNG_H_
