using UnityEngine;

namespace Barely {
  [DisallowMultipleComponent]
  public class GlobalSettings : MonoBehaviour {
    [Tooltip("Sets the global tempo in beats per minute.")]
    [Range(30.0f, 960.0f)]
    public float Bpm = 120.0f;

    [Header("Compressor")]

    [Range(0.0f, 1.0f)]
    public float CompMix = 1.0f;

    [Range(0.0f, 8.0f)]
    public float CompAttack = 0.0f;

    [Range(0.0f, 8.0f)]
    public float CompRelease = 0.0f;

    [Range(0.0f, 1.0f)]
    public float CompThreshold = 1.0f;

    [Range(0.0f, 1.0f)]
    public float CompRatio = 0.0f;

    [Header("Delay")]

    [Range(0.0f, 1.0f)]
    public float DelayMix = 1.0f;

    [Range(0.0f, 8.0f)]
    public float DelayTime = 0.0f;

    [Range(0.0f, 1.0f)]
    public float DelayFeedback = 0.0f;

    [Range(0.0f, 1.0f)]
    public float DelayLowPassCutoff = 1.0f;

    [Range(0.0f, 1.0f)]
    public float DelayHighPassCutoff = 0.0f;

    [Range(0.0f, 2.0f)]
    public float DelayReverbSend = 0.0f;

    [Header("Reverb")]

    [Range(0.0f, 1.0f)]
    public float ReverbMix = 1.0f;

    [Range(0.0f, 1.0f)]
    public float ReverbDamping = 0.0f;

    [Range(0.0f, 1.0f)]
    public float ReverbRoomSize = 0.0f;

    [Range(0.0f, 1.0f)]
    public float ReverbStereoWidth = 1.0f;

    public bool ReverbFreeze = false;

    [Header("Sidechain")]

    [Range(0.0f, 1.0f)]
    public float SidechainMix = 1.0f;

    [Range(0.0f, 8.0f)]
    public float SidechainAttack = 0.0f;

    [Range(0.0f, 8.0f)]
    public float SidechainRelease = 0.0f;

    [Range(0.0f, 1.0f)]
    public float SidechainThreshold = 1.0f;

    [Range(0.0f, 1.0f)]
    public float SidechainRatio = 0.0f;

    private void Update() {
      Engine.Tempo = (double)Bpm;
      Engine.CompMix = CompMix;
      Engine.CompAttack = CompAttack;
      Engine.CompRelease = CompRelease;
      Engine.CompThreshold = CompThreshold;
      Engine.CompRatio = CompRatio;
      Engine.DelayMix = DelayMix;
      Engine.DelayTime = DelayTime;
      Engine.DelayFeedback = DelayFeedback;
      Engine.DelayLowPassCutoff = DelayLowPassCutoff;
      Engine.DelayHighPassCutoff = DelayHighPassCutoff;
      Engine.DelayReverbSend = DelayReverbSend;
      Engine.ReverbMix = ReverbMix;
      Engine.ReverbDamping = ReverbDamping;
      Engine.ReverbRoomSize = ReverbRoomSize;
      Engine.ReverbStereoWidth = ReverbStereoWidth;
      Engine.ReverbFreeze = ReverbFreeze;
      Engine.SidechainMix = SidechainMix;
      Engine.SidechainAttack = SidechainAttack;
      Engine.SidechainRelease = SidechainRelease;
      Engine.SidechainThreshold = SidechainThreshold;
      Engine.SidechainRatio = SidechainRatio;
    }
  }
}  // namespace Barely
