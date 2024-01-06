using System;
using UnityEngine;
using UnityEngine.Assertions;

namespace Barely {
  /// Representation of a rational number.
  [Serializable]
  public struct Rational {
    public Rational(int numerator = 0, int denominator = 1) {
      Numerator = numerator;
      Denominator = denominator;
      Assert.AreNotEqual(Denominator, 0);
    }

    public static explicit operator double(Rational value) => (double)(value.Numerator) /
                                                              (double)(value.Denominator);

    /// Fractional numerator.
    public int Numerator;

    /// Fractional denominator.
    [Min(1)]
    public int Denominator;
  }
}  // namespace Barely
