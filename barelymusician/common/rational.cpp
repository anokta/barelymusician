#include "barelymusician/common/rational.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "barelymusician/barelymusician.h"

namespace barely {

namespace {

// Builds a normalized rational number from a given `numerator` and `denominator`.
Rational BuildNormalizedRational(std::int64_t numerator, std::int64_t denominator) noexcept {
  // Find the greatest common divisor.
  std::int64_t gcd = std::abs(numerator);
  std::int64_t divisor = std::abs(denominator);
  do {
    const std::int64_t remainder = gcd % divisor;
    gcd = divisor;
    divisor = remainder;
  } while (divisor > 0);
  // Normalize the rational number.
  return {static_cast<int>(numerator / gcd), static_cast<int>(denominator / gcd)};
}

}  // namespace

bool operator==(const Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.numerator == rhs.numerator && lhs.denominator == rhs.denominator) {
    return true;
  }
  return static_cast<std::int64_t>(lhs.numerator) * static_cast<std::int64_t>(rhs.denominator) ==
         static_cast<std::int64_t>(lhs.denominator) * static_cast<std::int64_t>(rhs.numerator);
}

bool operator!=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs == rhs); }

bool operator<(const Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.denominator == rhs.denominator) {
    return lhs.numerator < rhs.numerator;
  }
  return static_cast<std::int64_t>(lhs.numerator) * static_cast<std::int64_t>(rhs.denominator) <
         static_cast<std::int64_t>(lhs.denominator) * static_cast<std::int64_t>(rhs.numerator);
}

bool operator>(const Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.denominator == rhs.denominator) {
    return lhs.numerator > rhs.numerator;
  }
  return static_cast<std::int64_t>(lhs.numerator) * static_cast<std::int64_t>(rhs.denominator) >
         static_cast<std::int64_t>(lhs.denominator) * static_cast<std::int64_t>(rhs.numerator);
}

bool operator<=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs > rhs); }

bool operator>=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs < rhs); }

Rational& operator+=(Rational& lhs, const Rational& rhs) noexcept {
  const std::int64_t lhs_numerator = static_cast<std::int64_t>(lhs.numerator);
  const std::int64_t rhs_numerator = static_cast<std::int64_t>(rhs.numerator);
  const std::int64_t lhs_denominator = static_cast<std::int64_t>(lhs.denominator);
  if (lhs.denominator == rhs.denominator) {
    lhs = BuildNormalizedRational(lhs_numerator + rhs_numerator, lhs_denominator);
  } else {
    const std::int64_t rhs_denominator = static_cast<std::int64_t>(rhs.denominator);
    lhs = BuildNormalizedRational(lhs_numerator * rhs_denominator + rhs_numerator * lhs_denominator,
                                  lhs_denominator * rhs_denominator);
  }
  return lhs;
}

Rational& operator-=(Rational& lhs, const Rational& rhs) noexcept {
  const std::int64_t lhs_numerator = static_cast<std::int64_t>(lhs.numerator);
  const std::int64_t rhs_numerator = static_cast<std::int64_t>(rhs.numerator);
  const std::int64_t lhs_denominator = static_cast<std::int64_t>(lhs.denominator);
  if (lhs.denominator == rhs.denominator) {
    lhs = BuildNormalizedRational(lhs_numerator - rhs_numerator, lhs_denominator);
  } else {
    const std::int64_t rhs_denominator = static_cast<std::int64_t>(rhs.denominator);
    lhs = BuildNormalizedRational(lhs_numerator * rhs_denominator - rhs_numerator * lhs_denominator,
                                  lhs_denominator * rhs_denominator);
  }
  return lhs;
}

Rational& operator*=(Rational& lhs, const Rational& rhs) noexcept {
  lhs = BuildNormalizedRational(
      static_cast<std::int64_t>(lhs.numerator) * static_cast<std::int64_t>(rhs.numerator),
      static_cast<std::int64_t>(lhs.denominator) * static_cast<std::int64_t>(rhs.denominator));
  return lhs;
}

Rational& operator/=(Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.numerator == rhs.numerator) {
    lhs.numerator = rhs.denominator;
    if (lhs.numerator == lhs.denominator) {
      lhs.numerator = 1;
      lhs.denominator = 1;
    }
  } else if (lhs.denominator == rhs.denominator) {
    lhs.denominator = rhs.numerator;
    if (lhs.numerator == lhs.denominator) {
      lhs.numerator = 1;
      lhs.denominator = 1;
    }
  } else {
    lhs = BuildNormalizedRational(
        static_cast<std::int64_t>(lhs.numerator) * static_cast<std::int64_t>(rhs.denominator),
        static_cast<std::int64_t>(lhs.denominator) * static_cast<std::int64_t>(rhs.numerator));
  }
  return lhs;
}

Rational& operator+=(Rational& lhs, const int& rhs) noexcept {
  lhs = BuildNormalizedRational(
      static_cast<std::int64_t>(lhs.numerator) +
          static_cast<std::int64_t>(rhs) * static_cast<std::int64_t>(lhs.denominator),
      static_cast<std::int64_t>(lhs.denominator));
  return lhs;
}

Rational& operator-=(Rational& lhs, const int& rhs) noexcept {
  lhs = BuildNormalizedRational(
      static_cast<std::int64_t>(lhs.numerator) -
          static_cast<std::int64_t>(rhs) * static_cast<std::int64_t>(lhs.denominator),
      static_cast<std::int64_t>(lhs.denominator));
  return lhs;
}

Rational& operator*=(Rational& lhs, const int& rhs) noexcept {
  lhs = BuildNormalizedRational(
      static_cast<std::int64_t>(lhs.numerator) * static_cast<std::int64_t>(rhs),
      static_cast<std::int64_t>(lhs.denominator));
  return lhs;
}

Rational& operator/=(Rational& lhs, const int& rhs) noexcept {
  assert(rhs != 0);
  lhs = BuildNormalizedRational(
      static_cast<std::int64_t>(lhs.numerator),
      static_cast<std::int64_t>(lhs.denominator) * static_cast<std::int64_t>(rhs));
  return lhs;
}

Rational operator+(Rational lhs, const Rational& rhs) noexcept {
  lhs += rhs;
  return lhs;
}

Rational operator-(Rational lhs, const Rational& rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

Rational operator*(Rational lhs, const Rational& rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

Rational operator/(Rational lhs, const Rational& rhs) noexcept {
  lhs /= rhs;
  return lhs;
}

Rational operator+(Rational lhs, const int& rhs) noexcept {
  lhs += rhs;
  return lhs;
}

Rational operator-(Rational lhs, const int& rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

Rational operator*(Rational lhs, const int& rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

Rational operator/(Rational lhs, const int& rhs) noexcept {
  lhs /= rhs;
  return lhs;
}

std::ostream& operator<<(std::ostream& out_stream, const Rational& rational) {
  out_stream << (rational.denominator > 0 ? rational.numerator : -rational.numerator);
  if (rational.denominator != 1) {
    out_stream << "/" << std::abs(rational.denominator);
  }
  return out_stream;
}

}  // namespace barely
