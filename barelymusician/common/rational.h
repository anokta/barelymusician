#ifndef BARELYMUSICIAN_COMMON_RATIONAL_H_
#define BARELYMUSICIAN_COMMON_RATIONAL_H_

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

Rational& operator+=(Rational& lhs, const int& rhs) noexcept;
Rational& operator-=(Rational& lhs, const int& rhs) noexcept;
Rational& operator*=(Rational& lhs, const int& rhs) noexcept;
Rational& operator/=(Rational& lhs, const int& rhs) noexcept;

/// Other arithmetic operators.
Rational operator+(Rational lhs, const Rational& rhs) noexcept;
Rational operator-(Rational lhs, const Rational& rhs) noexcept;
Rational operator*(Rational lhs, const Rational& rhs) noexcept;
Rational operator/(Rational lhs, const Rational& rhs) noexcept;

Rational operator+(Rational lhs, const int& rhs) noexcept;
Rational operator-(Rational lhs, const int& rhs) noexcept;
Rational operator*(Rational lhs, const int& rhs) noexcept;
Rational operator/(Rational lhs, const int& rhs) noexcept;

/// Output stream operator.
std::ostream& operator<<(std::ostream& out_stream, const Rational& rational);

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_RATIONAL_H_
