using UnityEngine;

namespace Barely {
  /// A representation of a simple high-pass effect.
  public class HighPassEffect : Effect {
    /// Cutoff frequency.
    [Range(0, 48000)]
    public double CutoffFrequency = 0.0;

    private void Update() {
      SetControl(0, CutoffFrequency);
    }
  }
}  // namespace Barely
