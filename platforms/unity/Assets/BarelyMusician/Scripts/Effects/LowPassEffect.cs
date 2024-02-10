using UnityEngine;

namespace Barely {
  /// A representation of a simple low-pass effect.
  public class LowPassEffect : Effect {
    /// Cutoff frequency.
    public double CutoffFrequency {
      get { return GetControl(0); }
      set { SetControl(0, value, 0.0); }
    }

    /// Sets the cutoff frequency with a slope.
    ///
    /// @param cutoffFrequency Cutoff frequency.
    /// @param slopePerSecond Slope in value change per second.
    public void SetCutoffFrequency(double cutoffFrequency, double slopePerSecond) {
      SetControl(0, cutoffFrequency, slopePerSecond);
    }
  }
}  // namespace Barely
