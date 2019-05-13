#ifndef BARELYMUSICIAN_BASE_RANDOM_H_
#define BARELYMUSICIAN_BASE_RANDOM_H_

#include <random>

namespace barelyapi {

// Pseudo-random number generator.
class Random {
 public:
  // Draws a number with normal distribution.
  //
  // @param mean Distrubition mean value.
  // @param variance Distrubition variance.
  // @return Random real number.
  static float Normal(float mean, float variance);

  // Resets the random number generator with a new seed.
  //
  // @param seed Seed value to reset the generator with.
  static void SetSeed(int seed);

  // Draws a number with continuous uniform distribution in range [0, 1).
  //
  // @return Random real number.
  static float Uniform();

  // Draws a number with continuous uniform distribution in range [min, max).
  //
  // @param min Minimum value (inclusive).
  // @param max Maximum value (exclusive).
  // @return Random real number.
  static float Uniform(float min, float max);

  // Draws a number with discrete uniform distribution in range [min, max].
  // @param min Minimum value (inclusive).
  // @param max Maximum value (inclusive).
  // @return Random integer number.
  static int Uniform(int min, int max);

 private:
  // Random number generator engine.
  static std::default_random_engine generator_;

  // Normal distribution.
  static std::normal_distribution<float> normal_distribution_;

  // Uniform floating-point distribution.
  static std::uniform_real_distribution<float> uniform_float_distribution_;

  // Uniform integer distribution.
  static std::uniform_int_distribution<int> uniform_int_distribution_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_RANDOM_H_
