using System;
using System.Collections.Generic;
using System.Linq;
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
    /// Sampler.
    [Serializable]
    public class Sampler {
      /// Note pitch.
      public int Pitch = 0;

      /// Sample.
      public AudioClip Sample = null;

      /// Internal data.
      public double[] Data {
        get {
          if (_data == null || HasChanged) {
            _pitch = Pitch / 12.0;
            _sample = Sample;
            if (_sample == null || _sample.samples == 0) {
              _data = null;
              return _data;
            }
            if (_data == null || _sample.samples + 3 != Data.Length) {
              _data = new double[_sample.samples + 3];
            }
            // Write the meta data.
            _data[0] = _pitch;
            _data[1] = (double)_sample.frequency;
            _data[2] = (double)_sample.samples;
            // Write the sample data.
            float[] sampleData = new float[_sample.samples * _sample.channels];
            _sample.GetData(sampleData, 0);
            for (int frame = 0; frame < _sample.samples; ++frame) {
              _data[frame + 3] = 0.0;
              for (int channel = 0; channel < _sample.channels; ++channel) {
                _data[frame + 3] += (double)sampleData[frame * _sample.channels + channel];
              }
            }
          }
          return _data;
        }
      }
      private double[] _data = null;

      /// Denotes whether any changes has occured since the last update.
      public bool HasChanged {
        get { return Sample != _sample || Pitch / 12.0 != _pitch; }
      }

      /// Current note.
      private double _pitch = 0.0;

      // Current sample.
      private AudioClip _sample = null;
    }

    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Number of voices.
    [Range(1, 64)]
    public int VoiceCount = 8;

    /// Oscillator type.
    public OscillatorType OscillatorType = OscillatorType.SINE;

    /// List of samplers.
    public List<Sampler> Samplers = null;
    private int _samplerCount = 0;

    /// Sample player loop.
    public bool SamplerLoop = false;

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

    /// Pitch shift.
    [Range(-1.0f, 1.0f)]
    public double PitchShift = 0.0;

    private void Update() {
      if (Samplers != null) {
        UpdateSamplerData();
      }
      int id = 0;
      SetControl(id++, Gain);
      SetControl(id++, (double)VoiceCount);
      SetControl(id++, OscillatorType != OscillatorType.NONE ? 1.0 : 0.0);
      SetControl(id++, OscillatorType != OscillatorType.NONE ? (double)OscillatorType : 0.0);
      SetControl(id++, SamplerLoop ? 1.0 : 0.0);
      SetControl(id++, Attack);
      SetControl(id++, Decay);
      SetControl(id++, Sustain);
      SetControl(id++, Release);
      SetControl(id++, PitchShift);
    }

    private void UpdateSamplerData() {
      if (Samplers.Count == _samplerCount && !Samplers.Any(sampler => sampler.HasChanged)) {
        return;
      }
      _samplerCount = 0;
      int length = 1;
      for (int i = 0; i < Samplers.Count; ++i) {
        if (Samplers[i].Data != null) {
          ++_samplerCount;
          length += Samplers[i].Data.Length;
        }
      }
      // Write into an unmanaged pointer.
      int size = length * sizeof(double);
      IntPtr dataPtr = Marshal.AllocHGlobal(size);
      Marshal.Copy(new double[1] { (double)_samplerCount }, 0, dataPtr, 1);
      IntPtr samplerDataPtr = IntPtr.Add(dataPtr, sizeof(double));
      for (int i = 0; i < Samplers.Count; ++i) {
        double[] samplerData = Samplers[i].Data;
        if (samplerData != null) {
          Marshal.Copy(samplerData, 0, samplerDataPtr, samplerData.Length);
          samplerDataPtr = IntPtr.Add(samplerDataPtr, samplerData.Length * sizeof(double));
        }
      }
      SetData(dataPtr, size);
      Marshal.FreeHGlobal(dataPtr);
    }
  }
}  // namespace Barely
