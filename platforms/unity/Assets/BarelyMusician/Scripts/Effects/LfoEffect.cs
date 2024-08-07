using UnityEngine;

namespace Barely {
  /// A representation of a simple amplitude low-frequency oscillator effect.
  public class LfoEffect : Effect {
    /// Type.
    public OscillatorType OscillatorType = OscillatorType.SINE;

    /// Frequency.
    [Range(0.0f, 32.0f)]
    public double Frequency = 1.0;

    /// Intensity.
    [Range(0.0f, 1.0f)]
    public double Intensity = 1.0;

    private void Update() {
      GetControl(0).Value = (double)OscillatorType;
      GetControl(1).Value = Frequency;
      GetControl(2).Value = Intensity;
    }
  }
}  // namespace Barely
