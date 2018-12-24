#ifndef BARELYMUSICIAN_BASE_RANDOM_H_
#define BARELYMUSICIAN_BASE_RANDOM_H_

namespace barelyapi {
namespace random {

// Draws a number with normal distribution.
//
// @param mean Distrubition mean value.
// @param variance Distrubition variance.
// @return Random real number.
float Normal(float mean, float variance);

// Resets the random number generator with a new seed.
//
// @param seed Seed value to reset the generator with.
void Reset(int seed);

// Draws a number with continuous uniform distribution in range [0, 1).
//
// @return Random real number.
float Uniform();

// Draws a number with continuous uniform distribution in range [min, max).
//
// @param min Minimum value (inclusive).
// @param max Maximum value (exclusive).
// @return Random real number.
float Uniform(float min, float max);

// Draws a number with discrete uniform distribution in range [min, max].
// @param min Minimum value (inclusive).
// @param max Maximum value (inclusive).
// @return Random integer number.
int Uniform(int min, int max);

}  // namespace random
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_RANDOM_H_
