using System;
using UnityEngine;

namespace Barely {
  /// Simple polyphonic sampler instrument.
  public class SamplerInstrument : Instrument {
    /// Sample.
    public AudioClip Sample = null;

    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Root pitch.
    public double RootPitch = 0.0;

    /// Sample player loop.
    public bool Loop = false;

    /// Envelope attack.
    [Range(0.0f, 60.0f)]
    public double Attack = 0.05;

    /// Envelope decay.
    [Range(0.0f, 60.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release.
    [Range(0.0f, 60.0f)]
    public double Release = 0.25;

    /// Number of voices.
    [Range(1, 64)]
    public int VoiceCount = 8;

    // Current sample.
    private AudioClip _sample = null;

    private byte[] GetSampleData() {
      if (_sample == null || _sample.samples == 0 || _sample.channels != 1) {
        return null;
      }
      float[] sampleData = new float[_sample.samples];
      _sample.GetData(sampleData, 0);
      byte[] sampleDataBytes = new byte[sizeof(double) * sampleData.Length];
      for (int i = 0; i < sampleData.Length; ++i) {
        byte[] bytes = BitConverter.GetBytes((double)sampleData[i]);
        for (int byteIndex = 0; byteIndex < bytes.Length; ++byteIndex) {
          sampleDataBytes[i * bytes.Length + byteIndex] = bytes[byteIndex];
        }
      }
      return sampleDataBytes;
    }

    private void Update() {
      if (_sample != Sample) {
        _sample = Sample;
        SetData(GetSampleData());
      }
      SetControl(0, Gain);
      SetControl(1, RootPitch);
      SetControl(2, Loop ? 1.0 : 0.0);
      SetControl(3, Attack);
      SetControl(4, Decay);
      SetControl(5, Sustain);
      SetControl(6, Release);
      SetControl(7, (double)VoiceCount);
    }
  }
}
