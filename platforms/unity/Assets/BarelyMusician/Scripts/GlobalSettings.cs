using UnityEngine;

namespace Barely {
  [DisallowMultipleComponent]
  public class GlobalSettings : MonoBehaviour {
    [Tooltip("Sets the global tempo in beats per minute.")]
    [Range(30.0f, 960.0f)]
    public double Bpm = 120.0;

    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    [Header("Compressor")]

    [Range(0.0f, 1.0f)]
    public double CompMix = 1.0;

    [Range(0.0f, 8.0f)]
    public double CompAttack = 0.0;

    [Range(0.0f, 8.0f)]
    public double CompRelease = 0.0;

    [Range(0.0f, 1.0f)]
    public double CompThreshold = 1.0;

    [Range(0.0f, 1.0f)]
    public double CompRatio = 0.0;

    [Header("Delay")]

    [Range(0.0f, 1.0f)]
    public double DelayMix = 1.0;

    [Range(0.0f, 8.0f)]
    public double DelayTime = 0.0;

    [Range(0.0f, 1.0f)]
    public double DelayFeedback = 0.0;

    [Range(0.0f, 1.0f)]
    public double DelayLpfCutoff = 1.0;

    [Range(0.0f, 1.0f)]
    public double DelayHpfCutoff = 0.0;

    [Range(0.0f, 1.0f)]
    public double DelayPingPong = 0.0;

    [Range(0.0f, 2.0f)]
    public double DelayReverbSend = 0.0;

    [Header("Reverb")]

    [Range(0.0f, 1.0f)]
    public double ReverbMix = 1.0;

    [Range(0.0f, 1.0f)]
    public double ReverbDamping = 0.0;

    [Range(0.0f, 1.0f)]
    public double ReverbRoomSize = 0.0;

    [Range(0.0f, 1.0f)]
    public double ReverbStereoWidth = 1.0;

    public bool ReverbFreeze = false;

    [Header("Sidechain")]

    [Range(0.0f, 1.0f)]
    public double SidechainMix = 1.0;

    [Range(0.0f, 8.0f)]
    public double SidechainAttack = 0.0;

    [Range(0.0f, 8.0f)]
    public double SidechainRelease = 0.0;

    [Range(0.0f, 1.0f)]
    public double SidechainThreshold = 1.0;

    [Range(0.0f, 1.0f)]
    public double SidechainRatio = 0.0;

    private void LateUpdate() {
      Engine.Speed = (double)Bpm / 60.0;
      Engine.Gain = Gain;
      Engine.CompMix = CompMix;
      Engine.CompAttack = CompAttack;
      Engine.CompRelease = CompRelease;
      Engine.CompThreshold = CompThreshold;
      Engine.CompRatio = CompRatio;
      Engine.DelayMix = DelayMix;
      Engine.DelayTime = DelayTime;
      Engine.DelayFeedback = DelayFeedback;
      Engine.DelayLpfCutoff = DelayLpfCutoff;
      Engine.DelayHpfCutoff = DelayHpfCutoff;
      Engine.DelayPingPong = DelayPingPong;
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
