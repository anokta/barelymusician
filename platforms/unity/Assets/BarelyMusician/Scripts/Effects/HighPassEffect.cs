using UnityEngine;

namespace Barely {
  /// A representation of a simple high-pass effect.
  public class HighPassEffect : Effect {
    /// Cutoff frequency.
    public double CutoffFrequency {
      get { return GetControl(0); }
      set { SetControl(0, value, 0.0); }
    }

    /// Sets the cutoff frequency with a slope.
    ///
    /// @param cutoffFrequency Cutoff frequency.
    /// @param slopePerBeat Slope in value change per beat.
    public void SetCutoffFrequency(double cutoffFrequency, double slopePerBeat) {
      SetControl(0, cutoffFrequency, slopePerBeat);
    }
  }
}  // namespace Barely
