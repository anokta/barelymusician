using UnityEngine;

namespace Barely {
  /// A representation of a simple low-pass effect.
  public class LowPassEffect : Effect {
    /// Cutoff frequency.
    [Range(0.0f, 48000.0f)]
    public double CutoffFrequency = 48000.0;

    private void Update() {
      GetControl(0).Value = CutoffFrequency;
    }
  }
}  // namespace Barely
