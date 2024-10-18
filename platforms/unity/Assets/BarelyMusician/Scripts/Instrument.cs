using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Synthesizer oscillator type.
  public enum OscillatorType {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// Sine wave.
    [InspectorName("Sine")] SINE,
    /// Sawtooth wave.
    [InspectorName("Saw")] SAW,
    /// Square wave.
    [InspectorName("Square")] SQUARE,
    /// White noise.
    [InspectorName("Noise")] NOISE,
  }

  /// A representation of a musical instrument that can be played in real-time.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    /// Sampler.
    [Serializable]
    public class Sampler {
      /// Root note pitch.
      public int RootPitch = 0;

      /// Sample.
      public AudioClip Sample = null;

      /// Internal data.
      public double[] Data {
        get {
          if (_data == null || HasChanged) {
            _rootPitch = RootPitch;
            _sample = Sample;
            if (_sample == null || _sample.samples == 0) {
              _data = null;
              return _data;
            }
            if (_data == null || _sample.samples != _data.Length) {
              _data = new double[_sample.samples];
            }
            // Write the sample data.
            float[] sampleData = new float[_sample.samples * _sample.channels];
            _sample.GetData(sampleData, 0);
            for (int frame = 0; frame < _sample.samples; ++frame) {
              _data[frame] = 0.0;
              for (int channel = 0; channel < _sample.channels; ++channel) {
                _data[frame] += (double)sampleData[frame * _sample.channels + channel];
              }
            }
          }
          return _data;
        }
      }
      private double[] _data = null;

      /// Denotes whether any changes has occured since the last update.
      public bool HasChanged {
        get { return Sample != _sample || RootPitch != _rootPitch; }
      }

      /// Current root pitch.
      private double _rootPitch = 0.0;

      // Current sample.
      private AudioClip _sample = null;
    }

    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Number of voices.
    [Range(1, 32)]
    public int VoiceCount = 8;

    /// Oscillator type.
    public OscillatorType OscillatorType = OscillatorType.NONE;

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

    /// Retrigger.
    public bool Retrigger = false;

    /// Note off event callback.
    ///
    /// @param pitch Note pitch.
    public delegate void NoteOffEventCallback(double pitch);
    public event NoteOffEventCallback OnNoteOff;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on event callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public delegate void NoteOnEventCallback(double pitch, double intensity);
    public event NoteOnEventCallback OnNoteOn;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<float, float> {}
    public NoteOnEvent OnNoteOnEvent;

    /// Instrument create callback.
    public event Action OnInstrumentCreate;

    /// Instrument destroy callback.
    public event Action OnInstrumentDestroy;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Returns a control value.
    ///
    /// @param index Control index.
    /// @return Control value.
    public double GetControl(int index) {
      return Musician.Internal.Instrument_GetControl(_handle, index);
    }

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param index Control index.
    /// @return Control value.
    public double GetNoteControl(double pitch, int index) {
      return Musician.Internal.Instrument_GetNoteControl(_handle, pitch, index);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Instrument_IsNoteOn(_handle, pitch);
    }

    /// Resets a control value.
    ///
    /// @param index Control index.
    public void ResetControl(int index) {
      Musician.Internal.Instrument_ResetControl(_handle, index);
    }

    /// Resets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param index Note control index.
    public void ResetNoteControl(double pitch, int index) {
      Musician.Internal.Instrument_ResetNoteControl(_handle, pitch, index);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_handle);
    }

    /// Sets a control value.
    ///
    /// @param index Control index.
    /// @param value Control value.
    public void SetControl(int index, double value) {
      Musician.Internal.Instrument_SetControl(_handle, index, value);
    }

    /// Sets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param index Note control index.
    /// @param value Note control value.
    public void SetNoteControl(double pitch, int index, double value) {
      Musician.Internal.Instrument_SetNoteControl(_handle, pitch, index, value);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Internal.Instrument_SetNoteOff(_handle, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch, double intensity = 1.0) {
      Musician.Internal.Instrument_SetNoteOn(_handle, pitch, intensity);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the handle.
      public static IntPtr GetHandle(Instrument instrument) {
        return instrument ? instrument._handle : IntPtr.Zero;
      }

      /// Internal note off event callback.
      public static void OnNoteOffEvent(Instrument instrument, double pitch) {
        instrument.OnNoteOff?.Invoke(pitch);
        instrument.OnNoteOffEvent?.Invoke((float)pitch);
      }

      /// Internal note on event callback.
      public static void OnNoteOnEvent(Instrument instrument, double pitch, double intensity) {
        instrument.OnNoteOn?.Invoke(pitch, intensity);
        instrument.OnNoteOnEvent?.Invoke((float)pitch, (float)intensity);
      }
    }

    private void Awake() {
      Source = GetComponent<AudioSource>();
      Source.clip = AudioClip.Create("[DO NOT EDIT]", 64, 1, AudioSettings.outputSampleRate, false);
      float[] ones = new float[64];
      for (int i = 0; i < ones.Length; ++i) {
        ones[i] = 1.0f;
      }
      Source.clip.SetData(ones, 0);
      Source.loop = true;
    }

    private void OnDestroy() {
      Source = null;
    }

    private void OnEnable() {
      Musician.Internal.Instrument_Create(this, ref _handle);
      OnInstrumentCreate?.Invoke();
      Source?.Play();
    }

    private void OnDisable() {
      Source?.Stop();
      OnInstrumentDestroy?.Invoke();
      Musician.Internal.Instrument_Destroy(ref _handle);
    }

    private void Update() {
      UpdateSampleData();
      int index = 0;
      SetControl(index++, Gain);
      SetControl(index++, (double)VoiceCount);
      SetControl(index++, (double)OscillatorType);
      SetControl(index++, SamplerLoop ? 1.0 : 0.0);
      SetControl(index++, Attack);
      SetControl(index++, Decay);
      SetControl(index++, Sustain);
      SetControl(index++, Release);
      SetControl(index++, PitchShift);
      SetControl(index++, Retrigger ? 1.0 : 0.0);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_handle, data, channels);
    }

    private void UpdateSampleData() {
      if (Samplers != null &&
          (Samplers.Count != _samplerCount || Samplers.Any(sampler => sampler.HasChanged))) {
        _samplerCount = Samplers.Count;
        Musician.Internal.Instrument_SetSampleData(_handle, Samplers);
      }
    }

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
