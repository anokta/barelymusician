using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Assertions;

namespace Barely {
  /// Representation of a rational number.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct Rational {
    public Rational(int numerator = 0, int denominator = 1) {
      Numerator = numerator;
      Denominator = denominator;
      Assert.AreNotEqual(Denominator, 0);
    }

    public static explicit operator double(Rational value) => (double)value.Numerator
                                                              / value.Denominator;

    /// Fractional numerator.
    public Int64 Numerator;

    /// Fractional denominator.
    [Min(1)]
    public Int64 Denominator;
  }
}  // namespace Barely
