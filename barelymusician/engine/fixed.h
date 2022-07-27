#ifndef BARELYMUSICIAN_ENGINE_FIXED_H_
#define BARELYMUSICIAN_ENGINE_FIXED_H_

#include <compare>
#include <cstdint>

namespace barely::internal {

/// Class that wraps fixed-point value.
class Fixed {
 public:
  /// Default constructor.
  Fixed() = default;

  /// Constructs new `Fixed`.
  ///
  /// @param value Double-precision floating point value.
  explicit Fixed(double value) noexcept;

  /// Constructs new `Fixed`.
  ///
  /// @param value Integer value.
  explicit Fixed(int value) noexcept;

  /// Returns maximum fixed-point value.
  static Fixed GetMax();

  /// Returns corresponding double-precision floating point value.
  double ToDouble() const;

  /// Default comparator.
  auto operator<=>(const Fixed& other) const noexcept = default;

  /// Compound assignment operators.
  Fixed& operator+=(const Fixed& other) noexcept;
  Fixed& operator-=(const Fixed& other) noexcept;
  Fixed& operator*=(const Fixed& other) noexcept;
  Fixed& operator/=(const Fixed& other) noexcept;
  Fixed& operator%=(const Fixed& other) noexcept;

 private:
  // Raw value to represent fixed-point value with 16 fractional bits.
  std::int64_t raw_value_ = 0;
};

/// Other arithmetic operators.
Fixed operator+(Fixed lhs, const Fixed& rhs) noexcept;
Fixed operator-(Fixed lhs, const Fixed& rhs) noexcept;
Fixed operator*(Fixed lhs, const Fixed& rhs) noexcept;
Fixed operator/(Fixed lhs, const Fixed& rhs) noexcept;
Fixed operator%(Fixed lhs, const Fixed& rhs) noexcept;

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_FIXED_H_
