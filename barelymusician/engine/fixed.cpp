#include "barelymusician/engine/fixed.h"

#include <cmath>
#include <cstdint>
#include <limits>

namespace barely::internal {

namespace {

// Scale factor to convert an integer value to a fixed-point value.
constexpr double kScaleFactor = static_cast<double>(1 << 16);

}  // namespace

Fixed::Fixed(double value) noexcept
    : raw_value_(static_cast<std::int64_t>(
          std::round(value * static_cast<double>(kScaleFactor)))) {}

Fixed::Fixed(int value) noexcept : Fixed(static_cast<double>(value)) {}

Fixed Fixed::GetMax() {
  Fixed max_value = Fixed();
  max_value.raw_value_ = std::numeric_limits<std::int64_t>::max();
  return max_value;
}

double Fixed::ToDouble() const {
  return static_cast<double>(raw_value_) / kScaleFactor;
}

Fixed& Fixed::operator+=(const Fixed& other) noexcept {
  raw_value_ += other.raw_value_;
  return *this;
}

Fixed& Fixed::operator-=(const Fixed& other) noexcept {
  raw_value_ -= other.raw_value_;
  return *this;
}

Fixed& Fixed::operator*=(const Fixed& other) noexcept {
  raw_value_ *= other.raw_value_;
  return *this;
}

Fixed& Fixed::operator/=(const Fixed& other) noexcept {
  raw_value_ /= other.raw_value_;
  return *this;
}

Fixed& Fixed::operator%=(const Fixed& other) noexcept {
  raw_value_ %= other.raw_value_;
  return *this;
}

Fixed operator+(Fixed lhs, const Fixed& rhs) noexcept {
  lhs += rhs;
  return lhs;
}

Fixed operator-(Fixed lhs, const Fixed& rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

Fixed operator*(Fixed lhs, const Fixed& rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

Fixed operator/(Fixed lhs, const Fixed& rhs) noexcept {
  lhs /= rhs;
  return lhs;
}

Fixed operator%(Fixed lhs, const Fixed& rhs) noexcept {
  lhs %= rhs;
  return lhs;
}

}  // namespace barely::internal
