#include "barelymusician/common/rational.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "barelymusician/barelymusician.h"

namespace barely {

namespace {

// Normalizes a given `rational` number.
void Normalize(Rational& rational) noexcept {
  // Find the greatest common divisor.
  std::int64_t gcd = std::abs(rational.numerator);
  std::int64_t divisor = std::abs(rational.denominator);
  do {
    const std::int64_t remainder = gcd % divisor;
    gcd = divisor;
    divisor = remainder;
  } while (divisor > 0);
  // Normalize the rational number.
  rational.numerator /= gcd;
  rational.denominator /= gcd;
}

}  // namespace

bool operator==(const Rational& lhs, const Rational& rhs) noexcept {
  return (lhs.numerator == rhs.numerator && lhs.denominator == rhs.denominator) ||
         (lhs.numerator * rhs.denominator == lhs.denominator * rhs.numerator);
}

bool operator!=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs == rhs); }

bool operator<(const Rational& lhs, const Rational& rhs) noexcept {
  return (lhs.denominator == rhs.denominator && lhs.numerator < rhs.numerator) ||
         (lhs.numerator * rhs.denominator < lhs.denominator * rhs.numerator);
}

bool operator>(const Rational& lhs, const Rational& rhs) noexcept {
  return (lhs.denominator == rhs.denominator && lhs.numerator > rhs.numerator) ||
         (lhs.numerator * rhs.denominator > lhs.denominator * rhs.numerator);
}

bool operator<=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs > rhs); }

bool operator>=(const Rational& lhs, const Rational& rhs) noexcept { return !(lhs < rhs); }

Rational& operator+=(Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.denominator == rhs.denominator) {
    lhs.numerator += rhs.numerator;
  } else {
    lhs.numerator = lhs.numerator * rhs.denominator + rhs.numerator * lhs.denominator;
    lhs.denominator *= rhs.denominator;
  }
  Normalize(lhs);
  return lhs;
}

Rational& operator-=(Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.denominator == rhs.denominator) {
    lhs.numerator -= rhs.numerator;
  } else {
    lhs.numerator = lhs.numerator * rhs.denominator - rhs.numerator * lhs.denominator;
    lhs.denominator *= rhs.denominator;
  }
  Normalize(lhs);
  return lhs;
}

Rational& operator*=(Rational& lhs, const Rational& rhs) noexcept {
  lhs.numerator *= rhs.numerator;
  lhs.denominator *= rhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator/=(Rational& lhs, const Rational& rhs) noexcept {
  if (lhs.numerator == rhs.numerator && lhs.denominator == lhs.denominator) {
    lhs.numerator = 1;
    lhs.denominator = 1;
    return lhs;
  }
  if (lhs.numerator == rhs.numerator) {
    lhs.numerator = rhs.denominator;
  } else if (lhs.denominator == rhs.denominator) {
    lhs.denominator = rhs.numerator;
  } else {
    lhs.numerator *= rhs.denominator;
    lhs.denominator *= rhs.numerator;
  }
  Normalize(lhs);
  return lhs;
}

Rational& operator+=(Rational& lhs, const int& rhs) noexcept {
  lhs.numerator += static_cast<std::int64_t>(rhs) * lhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator-=(Rational& lhs, const int& rhs) noexcept {
  lhs.numerator -= static_cast<std::int64_t>(rhs) * lhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator*=(Rational& lhs, const int& rhs) noexcept {
  lhs.numerator *= static_cast<std::int64_t>(rhs);
  Normalize(lhs);
  return lhs;
}

Rational& operator/=(Rational& lhs, const int& rhs) noexcept {
  assert(rhs != 0);
  lhs.denominator *= static_cast<std::int64_t>(rhs);
  Normalize(lhs);
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
