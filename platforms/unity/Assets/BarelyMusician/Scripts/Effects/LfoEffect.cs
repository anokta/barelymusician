using UnityEngine;

namespace Barely {
  /// A representation of a simple amplitude low-frequency oscillator effect.
  public class LfoEffect : Effect {
    /// Type.
    public OscillatorType Type {
      get { return (OscillatorType)GetControl(0); }
      set { SetControl(0, (double)value, 0.0); }
    }

    /// Frequency.
    public double Frequency {
      get { return GetControl(1); }
      set { SetControl(1, value, 0.0); }
    }

    /// Intensity.
    public double Intensity {
      get { return GetControl(2); }
      set { SetControl(2, value, 0.0); }
    }

    /// Sets the oscillator frequency with a slope.
    ///
    /// @param frequency Oscillator frequency.
    /// @param slopePerBeat Slope in value change per beat.
    public void SetFrequency(double frequency, double slopePerBeat) {
      SetControl(0, frequency, slopePerBeat);
    }

    /// Sets the intensity with a slope.
    ///
    /// @param intensity Intensity.
    /// @param slopePerBeat Slope in value change per beat.
    public void SetIntensity(double intensity, double slopePerBeat) {
      SetControl(0, intensity, slopePerBeat);
    }
  }
}  // namespace Barely
