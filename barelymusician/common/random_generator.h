#ifndef BARELYMUSICIAN_COMMON_RANDOM_GENERATOR_H_
#define BARELYMUSICIAN_COMMON_RANDOM_GENERATOR_H_

#include <random>

namespace barelyapi {

/// Pseudo-random number generator.
class RandomGenerator {
 public:
  /// Constructs new |RandomGenerator|.
  ///
  /// @param seed Seed value.
  explicit RandomGenerator(int seed = 0);

  /// Draws a number with normal distribution.
  ///
  /// @param mean Distrubition mean value.
  /// @param variance Distrubition variance.
  /// @return Random double number.
  double DrawNormal(double mean, double variance);

  /// Draws a number with normal distribution.
  ///
  /// @param mean Distrubition mean value.
  /// @param variance Distrubition variance.
  /// @return Random float number.
  float DrawNormal(float mean, float variance);

  /// Draws a number with continuous uniform distribution in range [min, max).
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (exclusive).
  /// @return Random double number.
  double DrawUniform(double min, double max);

  /// Draws a number with continuous uniform distribution in range [min, max).
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (exclusive).
  /// @return Random float number.
  float DrawUniform(float min, float max);

  /// Draws a number with discrete uniform distribution in range [min, max].
  ///
  /// @param min Minimum value (inclusive).
  /// @param max Maximum value (inclusive).
  /// @return Random integer number.
  int DrawUniform(int min, int max);

  /// Resets the random number generator with a new seed.
  ///
  /// @param seed Seed value to reset the generator with.
  void Reset(int seed);

 private:
  // Pseudo-random number generator engine.
  std::default_random_engine engine_;

  // Normal distributions.
  std::normal_distribution<double> normal_double_distribution_;
  std::normal_distribution<float> normal_float_distribution_;

  // Uniform distributions.
  std::uniform_real_distribution<double> uniform_double_distribution_;
  std::uniform_real_distribution<float> uniform_float_distribution_;
  std::uniform_int_distribution<int> uniform_int_distribution_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMMON_RANDOM_GENERATOR_H_