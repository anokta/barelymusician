using UnityEngine;

namespace Barely {
  /// A representation of a simple low-pass effect.
  public class LowPassEffect : Effect {
    /// Cutoff frequency.
    public double CutoffFrequency {
      get { return GetControl(0); }
      set { SetControl(0, value); }
    }

    /// Sets the cutoff frequency.
    ///
    /// @param cutoffFrequency Cutoff frequency.
    public void SetCutoffFrequency(double cutoffFrequency) {
      SetControl(0, cutoffFrequency);
    }
  }
}  // namespace Barely
