using UnityEngine;

namespace Barely {
  /// A representation of a simple low-pass effect.
  public class LowPassEffect : Effect {
    /// Cutoff frequency.
    [Range(0, 48000)]
    public double CutoffFrequency = 48000.0;

    private void Update() {
      SetControl(0, CutoffFrequency);
    }
  }
}
