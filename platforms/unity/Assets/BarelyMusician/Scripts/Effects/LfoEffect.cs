using UnityEngine;

namespace Barely {
  /// A representation of a simple amplitude low-frequency oscillator effect.
  public class LfoEffect : Effect {
    /// Type.
    public OscillatorType Type {
      get { return (OscillatorType)GetControl(0); }
      set { SetControl(0, (double)value); }
    }

    /// Frequency.
    public double Frequency {
      get { return GetControl(1); }
      set { SetControl(1, value); }
    }

    /// Intensity.
    public double Intensity {
      get { return GetControl(2); }
      set { SetControl(2, value); }
    }

    /// Sets the oscillator frequency.
    ///
    /// @param frequency Oscillator frequency.
    public void SetFrequency(double frequency) {
      SetControl(0, frequency);
    }

    /// Sets the intensity.
    ///
    /// @param intensity Intensity.
    public void SetIntensity(double intensity) {
      SetControl(0, intensity);
    }
  }
}  // namespace Barely
