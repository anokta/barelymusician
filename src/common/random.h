#ifndef BARELYMUSICIAN_COMMON_RANDOM_H_
#define BARELYMUSICIAN_COMMON_RANDOM_H_

#include <random>

namespace barely::internal {

/// Class that wraps a random number generator.
class Random {
 public:
  /// Constructs a new `Random`.
  ///
  /// @param seed Seed value.
  explicit Random(int seed = static_cast<int>(std::default_random_engine::default_seed)) noexcept;

  /// Draws a number with normal distribution.
  ///
  /// @param mean Distrubition mean value.
  /// @param variance Distrubition variance.
  /// @return Random float number.
  float DrawNormal(float mean, float variance) noexcept;

  /// Draws a number with continuous uniform distribution in range [min, max).
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (exclusive).
  /// @return Random float number.
  float DrawUniform(float min, float max) noexcept;

  /// Draws a number with discrete uniform distribution in range [min, max].
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (inclusive).
  /// @return Random integer number.
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

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_COMMON_RANDOM_H_
