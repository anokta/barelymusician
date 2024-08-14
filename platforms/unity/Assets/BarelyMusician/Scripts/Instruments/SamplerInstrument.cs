using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of a simple sample-based polyphonic instrument with an envelope.
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

    /// Envelope attack in seconds.
    [Range(0.0f, 60.0f)]
    public double Attack = 0.05;

    /// Envelope decay in seconds.
    [Range(0.0f, 60.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release in seconds.
    [Range(0.0f, 60.0f)]
    public double Release = 0.25;

    /// Number of voices.
    [Range(1, 64)]
    public int VoiceCount = 8;

    // Current sample.
    private AudioClip _sample = null;

    protected override void OnEnable() {
      base.OnEnable();
      SetSampleData();
    }

    private void Update() {
      if (_sample != Sample) {
        SetSampleData();
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

    private void SetSampleData() {
      _sample = Sample;
      if (_sample == null || _sample.samples == 0) {
        SetData(IntPtr.Zero, 0);
        return;
      }
      double[] data = new double[_sample.samples + 1];
      // Write the frame rate.
      data[0] = (double)_sample.frequency;
      // Fill the sample data.
      float[] sampleData = new float[_sample.samples * _sample.channels];
      _sample.GetData(sampleData, 0);
      for (int frame = 0; frame < _sample.samples; ++frame) {
        for (int channel = 0; channel < _sample.channels; ++channel) {
          data[frame + 1] += (double)sampleData[frame * _sample.channels + channel];
        }
      }
      // Write into an unmanaged pointer.
      int size = data.Length * sizeof(double);
      IntPtr dataPtr = Marshal.AllocHGlobal(size);
      Marshal.Copy(data, 0, dataPtr, data.Length);
      SetData(dataPtr, size);
      Marshal.FreeHGlobal(dataPtr);
    }
  }
}  // namespace Barely
