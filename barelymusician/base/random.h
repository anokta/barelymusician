#ifndef BARELYMUSICIAN_BASE_RANDOM_H_
#define BARELYMUSICIAN_BASE_RANDOM_H_

#include <random>

namespace barelyapi {
namespace random {

// Draws a number with normal distribution.
//
// @param mean Distrubition mean value.
// @param variance Distrubition variance.
// @return Random double number.
double Normal(double mean, double variance);

// Draws a number with normal distribution.
//
// @param mean Distrubition mean value.
// @param variance Distrubition variance.
// @return Random float number.
float Normal(float mean, float variance);

// Resets the random number generator with a new seed.
//
// @param seed Seed value to reset the generator with.
void SetSeed(int seed);

// Draws a number with continuous uniform distribution in range [min, max).
//
// @param min Minimum value (inclusive).
// @param max Maximum value (exclusive).
// @return Random double number.
double Uniform(double min, double max);

// Draws a number with continuous uniform distribution in range [min, max).
//
// @param min Minimum value (inclusive).
// @param max Maximum value (exclusive).
// @return Random float number.
float Uniform(float min, float max);

// Draws a number with discrete uniform distribution in range [min, max].
// @param min Minimum value (inclusive).
// @param max Maximum value (inclusive).
// @return Random integer number.
int Uniform(int min, int max);

}  // namespace random
}  // namespace barelyapi

#endif  // BARELYMUSICIAN_BASE_RANDOM_H_
