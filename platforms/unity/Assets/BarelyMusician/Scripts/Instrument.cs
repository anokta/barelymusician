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

  /// A representation of a musical instrument that can be played in real-time.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
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

    /// Control event callback.
    ///
    /// @param id Control identifier.
    /// @param control Control value.
    public delegate void ControlEventCallback(int id, double value);
    public event ControlEventCallback OnControl;

    [Serializable]
    public class ControlEvent : UnityEngine.Events.UnityEvent<int, float> {}
    public ControlEvent OnControlEvent;

    /// Note control event callback.
    ///
    /// @param pitch Note pitch.
    /// @param id Note control identifier.
    /// @param control Note control value.
    public delegate void NoteControlEventCallback(double pitch, int id, double value);
    public event NoteControlEventCallback OnNoteControl;

    [Serializable]
    public class NoteControlEvent : UnityEngine.Events.UnityEvent<float, int, float> {}
    public NoteControlEvent OnNoteControlEvent;

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

    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Number of voices.
    [Range(1, 32)]
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

    /// Returns a control value.
    ///
    /// @param id Control identifier.
    /// @return Control value.
    public double GetControl(int id) {
      return Musician.Internal.Instrument_GetControl(_ptr, id);
    }

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param id Control identifier.
    /// @return Control value.
    public double GetNoteControl(double pitch, int id) {
      return Musician.Internal.Instrument_GetNoteControl(_ptr, pitch, id);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Instrument_IsNoteOn(_ptr, pitch);
    }

    /// Resets a control value.
    ///
    /// @param id Control identifier.
    public void ResetControl(int id) {
      Musician.Internal.Instrument_ResetControl(_ptr, id);
    }

    /// Resets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param id Note control identifier.
    public void ResetNoteControl(double pitch, int id) {
      Musician.Internal.Instrument_ResetNoteControl(_ptr, pitch, id);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_ptr);
    }

    /// Sets a control value.
    ///
    /// @param id Control identifier.
    /// @param value Control value.
    public void SetControl(int id, double value) {
      Musician.Internal.Instrument_SetControl(_ptr, id, value);
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Instrument_SetData(_ptr, dataPtr, size);
    }

    /// Sets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param id Note control identifier.
    /// @param value Note control value.
    public void SetNoteControl(double pitch, int id, double value) {
      Musician.Internal.Instrument_SetNoteControl(_ptr, pitch, id, value);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Internal.Instrument_SetNoteOff(_ptr, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch, double intensity = 1.0) {
      Musician.Internal.Instrument_SetNoteOn(_ptr, pitch, intensity);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the pointer.
      public static IntPtr GetPtr(Instrument instrument) {
        return instrument ? instrument._ptr : IntPtr.Zero;
      }

      /// Internal control event callback.
      public static void OnControlEvent(Instrument instrument, int id, double value) {
        instrument.OnControl?.Invoke(id, value);
        instrument.OnControlEvent?.Invoke(id, (float)value);
      }

      /// Internal note control event callback.
      public static void OnNoteControlEvent(Instrument instrument, double pitch, int id,
                                            double value) {
        instrument.OnNoteControl?.Invoke(pitch, id, value);
        instrument.OnNoteControlEvent?.Invoke((float)pitch, id, (float)value);
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
      Musician.Internal.Instrument_Create(this, ref _ptr);
      OnInstrumentCreate?.Invoke();
      Source?.Play();
    }

    private void OnDisable() {
      Source?.Stop();
      OnInstrumentDestroy?.Invoke();
      Musician.Internal.Instrument_Destroy(ref _ptr);
    }

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

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_ptr, data, channels);
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

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
