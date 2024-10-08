using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Synthesizer oscillator type.
  public enum OscillatorType {
    /// Sine wave.
    [InspectorName("Sine")] SINE = 0,
    /// Sawtooth wave.
    [InspectorName("Saw")] SAW = 1,
    /// Square wave.
    [InspectorName("Square")] SQUARE = 2,
    /// White noise.
    [InspectorName("Noise")] NOISE = 3,
    /// None.
    [InspectorName("None")] NONE = 4,
  }

  /// A representation of an ultimate instrument to conquer 'em all!
  public class UltimateInstrument : Instrument {
    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Number of voices.
    [Range(1, 64)]
    public int VoiceCount = 8;

    /// Oscillator type.
    public OscillatorType OscillatorType = OscillatorType.SINE;

    /// Sample.
    public AudioClip Sample = null;

    /// Sample player root note pitch.
    public double SampleRootPitch = 0.0;

    /// Sample player loop.
    public bool SampleLoop = false;

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

    protected override void OnEnable() {
      base.OnEnable();
      SetSampleData();
    }

    private void Update() {
      if (_sample != Sample || _sampleRootPitch != SampleRootPitch) {
        SetSampleData();
      }
      int id = 0;
      SetControl(id++, Gain);
      SetControl(id++, (double)VoiceCount);
      SetControl(id++, OscillatorType != OscillatorType.NONE ? 1.0 : 0.0);
      SetControl(id++, OscillatorType != OscillatorType.NONE ? (double)OscillatorType : 0.0);
      SetControl(id++, (_sample != null) ? 1.0 : 0.0);
      SetControl(id++, SampleLoop ? 1.0 : 0.0);
      SetControl(id++, Attack);
      SetControl(id++, Decay);
      SetControl(id++, Sustain);
      SetControl(id++, Release);
    }

    private void SetSampleData() {
      _sample = Sample;
      _sampleRootPitch = SampleRootPitch;
      if (_sample == null || _sample.samples == 0) {
        SetData(IntPtr.Zero, 0);
        return;
      }
      double[] data = new double[_sample.samples + 2];
      // Write the root pitch.
      data[0] = SampleRootPitch;
      // Write the frame rate.
      data[1] = (double)_sample.frequency;
      // Fill the sample data.
      float[] sampleData = new float[_sample.samples * _sample.channels];
      _sample.GetData(sampleData, 0);
      for (int frame = 0; frame < _sample.samples; ++frame) {
        for (int channel = 0; channel < _sample.channels; ++channel) {
          data[frame + 2] += (double)sampleData[frame * _sample.channels + channel];
        }
      }
      // Write into an unmanaged pointer.
      int size = data.Length * sizeof(double);
      IntPtr dataPtr = Marshal.AllocHGlobal(size);
      Marshal.Copy(data, 0, dataPtr, data.Length);
      SetData(dataPtr, size);
      Marshal.FreeHGlobal(dataPtr);
    }

    // Root pitch.
    private double _sampleRootPitch = 0.0;

    // Current sample.
    private AudioClip _sample = null;
  }
}  // namespace Barely
