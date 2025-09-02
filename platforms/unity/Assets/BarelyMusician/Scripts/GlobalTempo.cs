using UnityEngine;

namespace Barely {
  [DisallowMultipleComponent]
  public class GlobalTempo : MonoBehaviour {
    [Tooltip("Sets the global tempo in beats per minute.")]
    [Range(30.0f, 960.0f)]
    public float Bpm = 120.0f;

    [Range(0.0f, 1.0f)]
    public float DelayMix = 1.0f;

    [Range(0.0f, 10.0f)]
    public float DelayTime = 0.0f;

    [Range(0.0f, 1.0f)]
    public float DelayFeedback = 0.0f;

    private void Update() {
      Engine.Tempo = (double)Bpm;
      Engine.DelayMix = DelayMix;
      Engine.DelayTime = DelayTime;
      Engine.DelayFeedback = DelayFeedback;
    }
  }
}  // namespace Barely
