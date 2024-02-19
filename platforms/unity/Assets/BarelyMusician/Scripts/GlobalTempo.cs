using UnityEngine;

namespace Barely {
  [DisallowMultipleComponent]
  public class GlobalTempo : MonoBehaviour {
    [Tooltip("Sets the global tempo in beats per minute.")]
    [Range(30.0f, 960.0f)]
    public double Bpm = 120.0;

    private void Update() {
      Musician.Tempo = Bpm;
    }
  }
}  // namespace Barely
