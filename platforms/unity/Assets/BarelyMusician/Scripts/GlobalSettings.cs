using UnityEngine;

namespace Barely {
  [DisallowMultipleComponent]
  public class GlobalSettings : MonoBehaviour {
    [Tooltip("Sets the global tempo in beats per minute.")]
    [Range(30.0f, 960.0f)]
    public float Bpm = 120.0f;

    [Range(0.0f, 1.0f)]
    public float CompressorMix = 0.0f;

    [Range(0.001f, 1.0f)]
    public float CompressorAttack = 0.001f;

    [Range(0.01f, 3.0f)]
    public float CompressorRelease = 0.01f;

    [Range(0.0f, 1.0f)]
    public float CompressorThreshold = 1.0f;

    [Range(1.0f, 64.0f)]
    public float CompressorRatio = 1.0f;

    [Range(0.0f, 1.0f)]
    public float DelayMix = 1.0f;

    [Range(0.0f, 10.0f)]
    public float DelayTime = 0.0f;

    [Range(0.0f, 1.0f)]
    public float DelayFeedback = 0.0f;

    [Range(0.0f, 48000.0f)]
    public float DelayLowPassFrequency = 48000.0f;

    [Range(0.0f, 48000.0f)]
    public float DelayHighPassFrequency = 0.0f;

    [Range(0.0f, 1.0f)]
    public float DelayReverbSend = 0.0f;

    [Range(0.0f, 1.0f)]
    public float ReverbMix = 1.0f;

    [Range(0.0f, 0.4f)]
    public float ReverbDampingRatio = 0.0f;

    [Range(0.0f, 1.0f)]
    public float ReverbRoomSize = 0.0f;

    [Range(0.0f, 1.0f)]
    public float ReverbStereoWidth = 1.0f;

    public bool ReverbFreeze = false;

    [Range(0.0f, 1.0f)]
    public float SidechainMix = 1.0f;

    [Range(0.001f, 1.0f)]
    public float SidechainAttack = 0.001f;

    [Range(0.01f, 3.0f)]
    public float SidechainRelease = 0.01f;

    [Range(0.0f, 1.0f)]
    public float SidechainThreshold = 1.0f;

    [Range(1.0f, 64.0f)]
    public float SidechainRatio = 1.0f;

    private void Update() {
      Engine.Tempo = (double)Bpm;
      Engine.CompressorMix = CompressorMix;
      Engine.CompressorAttack = CompressorAttack;
      Engine.CompressorRelease = CompressorRelease;
      Engine.CompressorThreshold = CompressorThreshold;
      Engine.CompressorRatio = CompressorRatio;
      Engine.DelayMix = DelayMix;
      Engine.DelayTime = DelayTime;
      Engine.DelayFeedback = DelayFeedback;
      Engine.DelayLowPassFrequency = DelayLowPassFrequency;
      Engine.DelayHighPassFrequency = DelayHighPassFrequency;
      Engine.DelayReverbSend = DelayReverbSend;
      Engine.ReverbMix = ReverbMix;
      Engine.ReverbDampingRatio = ReverbDampingRatio;
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
