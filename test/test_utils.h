#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <cmath>

template <typename T>
bool ExpectEqual(const T& expected, const T& actual) {
  return expected == actual;
}

template <typename T>
bool ExpectNear(const T& expected, const T& actual, const T& epsilon) {
  return std::abs(expected - actual) < epsilon;
}

#endif  // TEST_UTILS_H_
