using UnityEngine;

namespace Barely {
  [DisallowMultipleComponent]
  public class GlobalTempo : MonoBehaviour {
    [Tooltip("Sets the global tempo in beats per minute.")]
    [Range(30.0f, 960.0f)]
    public float Bpm = 120.0f;

    private void Update() {
      Engine.Tempo = (double)Bpm;
    }
  }
}  // namespace Barely
