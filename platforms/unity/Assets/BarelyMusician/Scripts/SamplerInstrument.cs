using System;
using UnityEngine;

namespace Barely {
  /// Simple polyphonic sampler instrument.
  public class SamplerInstrument : Instrument {
    /// Sample.
    public AudioClip Sample = null;

    /// Root pitch.
    public double RootPitch = 0.0;

    /// Sample player loop.
    public bool Loop = false;

    /// Envelope attack.
    [Min(0.0f)]
    public double Attack = 0.05;

    /// Envelope decay.
    [Min(0.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release.
    [Min(0.0f)]
    public double Release = 0.25;

    /// Number of voices.
    [Range(1, 32)]
    public int VoiceCount = 8;

    // Current sample.
    private AudioClip _sample = null;

    // TODO(#105): Update `GetData` to support different sample frequencies.
    private const int SampleFrequency = 48000;
    private const int MaxSampleCount = 10 * SampleFrequency;
    private float[] _sampleData = new float[MaxSampleCount];

    private byte[] GetSampleData() {
      if (_sample.samples == 0 || _sample.channels != 1) {
        return null;
      }
      _sample.GetData(_sampleData, 0);

      int length = Mathf.Min(_sample.samples, MaxSampleCount);
      byte[] sampleDataBytes = new byte[sizeof(double) * length];
      for (int i = 0; i < length; ++i) {
        byte[] bytes = BitConverter.GetBytes((double)_sampleData[i]);
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
      SetControl(0, RootPitch);
      SetControl(1, Loop ? 1.0 : 0.0);
      SetControl(2, Attack);
      SetControl(3, Decay);
      SetControl(4, Sustain);
      SetControl(5, Release);
      SetControl(6, (double)VoiceCount);
    }
  }
}
