#include "barelymusician/common/rational.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>

#include "barelymusician/barelymusician.h"

namespace barely {

namespace {

// Normalizes a given `rational` number.
void Normalize(Rational& rational) noexcept {
  const std::int64_t gcd = std::gcd(rational.numerator, rational.denominator);
  rational.numerator /= gcd;
  rational.denominator /= gcd;
  if (rational.denominator < 0) {
    rational.numerator *= -1;
    rational.denominator *= -1;
  }
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
  lhs.numerator = lhs.numerator * rhs.denominator + rhs.numerator * lhs.denominator;
  lhs.denominator *= rhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator-=(Rational& lhs, const Rational& rhs) noexcept {
  lhs.numerator = lhs.numerator * rhs.denominator - rhs.numerator * lhs.denominator;
  lhs.denominator *= rhs.denominator;
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
  assert(rhs != 0);
  if (lhs == 0) {
    return lhs;
  }
  lhs.numerator *= rhs.denominator;
  lhs.denominator *= rhs.numerator;
  Normalize(lhs);
  return lhs;
}

Rational& operator%=(Rational& lhs, const Rational& rhs) noexcept {
  assert(rhs != 0);
  lhs.numerator *= rhs.denominator;
  lhs.numerator %= rhs.numerator * lhs.denominator;
  lhs.denominator *= rhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator+=(Rational& lhs, const std::int64_t& rhs) noexcept {
  lhs.numerator += static_cast<std::int64_t>(rhs) * lhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator-=(Rational& lhs, const std::int64_t& rhs) noexcept {
  lhs.numerator -= static_cast<std::int64_t>(rhs) * lhs.denominator;
  Normalize(lhs);
  return lhs;
}

Rational& operator*=(Rational& lhs, const std::int64_t& rhs) noexcept {
  lhs.numerator *= static_cast<std::int64_t>(rhs);
  Normalize(lhs);
  return lhs;
}

Rational& operator/=(Rational& lhs, const std::int64_t& rhs) noexcept {
  assert(rhs != 0);
  if (lhs == 0) {
    return lhs;
  }
  lhs.denominator *= static_cast<std::int64_t>(rhs);
  Normalize(lhs);
  return lhs;
}

Rational& operator%=(Rational& lhs, const std::int64_t& rhs) noexcept {
  assert(rhs != 0);
  lhs.numerator %= static_cast<std::int64_t>(rhs) * lhs.denominator;
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

Rational operator%(Rational lhs, const Rational& rhs) noexcept {
  lhs %= rhs;
  return lhs;
}

Rational operator+(Rational lhs, const std::int64_t& rhs) noexcept {
  lhs += rhs;
  return lhs;
}

Rational operator-(Rational lhs, const std::int64_t& rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

Rational operator*(Rational lhs, const std::int64_t& rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

Rational operator/(Rational lhs, const std::int64_t& rhs) noexcept {
  lhs /= rhs;
  return lhs;
}

Rational operator%(Rational lhs, const std::int64_t& rhs) noexcept {
  lhs %= rhs;
  return lhs;
}

Rational RationalNormalized(std::int64_t numerator, std::int64_t denominator) noexcept {
  Rational rational(numerator, denominator);
  Normalize(rational);
  return rational;
}

std::ostream& operator<<(std::ostream& out_stream, const Rational& rational) {
  out_stream << (rational.denominator > 0 ? rational.numerator : -rational.numerator);
  if (rational.numerator != 0 && rational.denominator != 1) {
    out_stream << "/" << std::abs(rational.denominator);
  }
  return out_stream;
}

}  // namespace barely
