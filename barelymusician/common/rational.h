#ifndef BARELYMUSICIAN_COMMON_RATIONAL_H_
#define BARELYMUSICIAN_COMMON_RATIONAL_H_

#include <cstdint>
#include <iostream>

#include "barelymusician/barelymusician.h"

namespace barely {

/// Comparators.
bool operator==(const Rational& lhs, const Rational& rhs) noexcept;
bool operator!=(const Rational& lhs, const Rational& rhs) noexcept;
bool operator<(const Rational& lhs, const Rational& rhs) noexcept;
bool operator>(const Rational& lhs, const Rational& rhs) noexcept;
bool operator<=(const Rational& lhs, const Rational& rhs) noexcept;
bool operator>=(const Rational& lhs, const Rational& rhs) noexcept;

/// Compound assignment operators.
Rational& operator+=(Rational& lhs, const Rational& rhs) noexcept;
Rational& operator-=(Rational& lhs, const Rational& rhs) noexcept;
Rational& operator*=(Rational& lhs, const Rational& rhs) noexcept;
Rational& operator/=(Rational& lhs, const Rational& rhs) noexcept;
Rational& operator%=(Rational& lhs, const Rational& rhs) noexcept;

Rational& operator+=(Rational& lhs, const std::int64_t& rhs) noexcept;
Rational& operator-=(Rational& lhs, const std::int64_t& rhs) noexcept;
Rational& operator*=(Rational& lhs, const std::int64_t& rhs) noexcept;
Rational& operator/=(Rational& lhs, const std::int64_t& rhs) noexcept;
Rational& operator%=(Rational& lhs, const std::int64_t& rhs) noexcept;

/// Other arithmetic operators.
Rational operator+(Rational lhs, const Rational& rhs) noexcept;
Rational operator-(Rational lhs, const Rational& rhs) noexcept;
Rational operator*(Rational lhs, const Rational& rhs) noexcept;
Rational operator/(Rational lhs, const Rational& rhs) noexcept;
Rational operator%(Rational lhs, const Rational& rhs) noexcept;

Rational operator+(Rational lhs, const std::int64_t& rhs) noexcept;
Rational operator-(Rational lhs, const std::int64_t& rhs) noexcept;
Rational operator*(Rational lhs, const std::int64_t& rhs) noexcept;
Rational operator/(Rational lhs, const std::int64_t& rhs) noexcept;
Rational operator%(Rational lhs, const std::int64_t& rhs) noexcept;

/// Output stream operator.
std::ostream& operator<<(std::ostream& out_stream, const Rational& rational);

/// Returns a normalized `Rational` number.
///
/// @param numerator Numerator.
/// @param denominator Denominator.
Rational RationalNormalized(std::int64_t numerator = 0, std::int64_t denominator = 1) noexcept;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_RATIONAL_H_
