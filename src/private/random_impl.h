#ifndef BARELYMUSICIAN_PRIVATE_RANDOM_IMPL_H_
#define BARELYMUSICIAN_PRIVATE_RANDOM_IMPL_H_

#include <random>

#include "barelymusician.h"

namespace barely {

/// Class that implements a random number generator.
class RandomImpl {
 public:
  /// Constructs a new `RandomImpl`.
  ///
  /// @param seed Seed value.
  explicit RandomImpl(
      int seed = static_cast<int>(std::default_random_engine::default_seed)) noexcept;

  /// Draws a number with normal distribution.
  ///
  /// @param mean Distrubition mean value.
  /// @param variance Distrubition variance.
  /// @return RandomImpl float number.
  float DrawNormal(float mean, float variance) noexcept;

  /// Draws a number with continuous uniform distribution in range [min, max).
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (exclusive).
  /// @return RandomImpl float number.
  float DrawUniform(float min, float max) noexcept;

  /// Draws a number with discrete uniform distribution in range [min, max].
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (inclusive).
  /// @return RandomImpl integer number.
  int DrawUniform(int min, int max) noexcept;

  /// Resets the random number generator with a new seed.
  ///
  /// @param seed Seed value to reset the generator with.
  void Reset(int seed) noexcept;

 private:
  // Pseudo-random number generator engine.
  std::default_random_engine engine_;

  // Distributions.
  std::normal_distribution<float> normal_distribution_;
  std::uniform_real_distribution<float> uniform_real_distribution_;
  std::uniform_int_distribution<int> uniform_int_distribution_;
};

}  // namespace barely

struct BarelyRandom : public barely::RandomImpl {};
static_assert(sizeof(BarelyRandom) == sizeof(barely::RandomImpl));

#endif  // BARELYMUSICIAN_PRIVATE_RANDOM_IMPL_H_
