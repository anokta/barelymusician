using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Assertions;

namespace Barely {
  /// Representation of a rational number.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct Rational {
    public Rational(Int64 numerator = 0, Int64 denominator = 1) {
      Numerator = numerator;
      Denominator = denominator;
      Assert.AreNotEqual(Denominator, 0);
    }

    public static explicit operator double(Rational value) => (double)value.Numerator
                                                              / value.Denominator;

    public static bool operator ==(Rational lhs, Rational rhs) =>
        (lhs.Numerator == rhs.Numerator && lhs.Denominator == rhs.Denominator) ||
        (lhs.Numerator * rhs.Denominator == lhs.Denominator * rhs.Numerator);

    public static bool operator !=(Rational lhs, Rational rhs) => !(lhs == rhs);

    public static bool operator<(Rational lhs, Rational rhs) =>
        (lhs.Denominator == rhs.Denominator && lhs.Numerator < rhs.Numerator) ||
        (lhs.Numerator * rhs.Denominator < lhs.Denominator * rhs.Numerator);

    public static bool operator>(Rational lhs, Rational rhs) =>
        (lhs.Denominator == rhs.Denominator && lhs.Numerator > rhs.Numerator) ||
        (lhs.Numerator * rhs.Denominator > lhs.Denominator * rhs.Numerator);

    public override bool Equals(System.Object other) => other != null &&
                                                        GetType().Equals(other.GetType()) &&
                                                        this == (Rational)other;

    public override int GetHashCode() => ((double)Numerator / Denominator).GetHashCode();

    /// Fractional numerator.
    public Int64 Numerator;

    /// Fractional denominator.
    [Min(1)]
    public Int64 Denominator;
  }
}  // namespace Barely
