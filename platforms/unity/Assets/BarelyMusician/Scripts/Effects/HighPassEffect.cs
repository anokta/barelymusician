using UnityEngine;

namespace Barely {
  /// A representation of a simple high-pass effect.
  public class HighPassEffect : Effect {
    /// Cutoff frequency.
    [Range(0.0f, 48000.0f)]
    public double CutoffFrequency = 0.0;

    private void Update() {
      GetControl(0).Value = CutoffFrequency;
    }
  }
}  // namespace Barely
