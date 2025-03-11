using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Barely {
  /// Filter type.
  public enum FilterType {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// Low-pass.
    [InspectorName("Low-pass")] LOW_PASS,
    /// High-pass.
    [InspectorName("High-pass")] HIGH_PASS,
  }

  /// Note control type.
  public enum NoteControlType {
    /// Pitch shift.
    [InspectorName("Pitch Shift")] PITCH_SHIFT = 0,
  }

  /// Oscillator mode.
  public enum OscMode {
    /// Mix.
    [InspectorName("Mix")] MIX = 0,
    /// Amplitude modulation by the oscillator applied to the slice.
    [InspectorName("AM")] AM,
    /// Aamplitude modulation by the slice applied to the oscillator.
    [InspectorName("Envelope Follower")] ENVELOPE_FOLLOWER,
    /// Frequency modulation by the oscillator applied to the slice.
    [InspectorName("FM")] FM,
    /// Frequency modulation by the slice applied to the oscillator.
    [InspectorName("MF")] MF,
    /// Ring modulation.
    [InspectorName("Ring")] RING,
  }

  /// Slice mode.
  public enum SliceMode {
    /// Sustain.
    [InspectorName("Sustain")] SUSTAIN = 0,
    /// Loop.
    [InspectorName("Loop")] LOOP,
    /// Once.
    [InspectorName("Once")] ONCE,
  }

  /// A representation of a musical instrument that can be played in real-time.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    /// Slice.
    [Serializable]
    public class Slice {
      /// Root note pitch.
      public int RootPitch = 0;

      /// Sample.
      public AudioClip Sample = null;

      /// Internal data.
      public float[] Data {
        get {
          if (_data == null || HasChanged) {
            _rootPitch = RootPitch;
            _sample = Sample;
            if (_sample == null || _sample.samples == 0) {
              _data = null;
              return _data;
            }
            if (_data == null || _sample.samples != _data.Length) {
              _data = new float[_sample.samples];
            }
            // Write the sample data.
            float[] sampleData = new float[_sample.samples * _sample.channels];
            _sample.GetData(sampleData, 0);
            for (int frame = 0; frame < _sample.samples; ++frame) {
              _data[frame] = 0.0f;
              for (int channel = 0; channel < _sample.channels; ++channel) {
                _data[frame] += sampleData[frame * _sample.channels + channel];
              }
            }
          }
          return _data;
        }
      }
      private float[] _data = null;

      /// Denotes whether any changes has occured since the last update.
      public bool HasChanged {
        get { return Sample != _sample || RootPitch != _rootPitch; }
      }

      /// Current root pitch.
      private float _rootPitch = 0.0f;

      // Current sample.
      private AudioClip _sample = null;
    }

    /// Gain in decibels.
    [Range(-80.0f, 0.0f)]
    public float Gain = 0.0f;

    /// Pitch shift.
    [Range(-2.0f, 2.0f)]
    public float PitchShift = 0.0f;

    /// Retrigger.
    public bool Retrigger = false;

    /// Number of voices.
    [Range(1, 20)]
    public int VoiceCount = 8;

    [Header("Envelope")]

    /// Envelope attack in seconds.
    [Range(0.0f, 60.0f)]
    public float Attack = 0.05f;

    /// Envelope decay in seconds.
    [Range(0.0f, 60.0f)]
    public float Decay = 0.0f;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public float Sustain = 1.0f;

    /// Envelope release in seconds.
    [Range(0.0f, 60.0f)]
    public float Release = 0.25f;

    [Header("Oscillator")]

    /// Oscillator mode.
    public OscMode OscMode = OscMode.MIX;

    /// Oscillator mix.
    [Range(0.0f, 1.0f)]
    public float OscMix = 0.0f;

    /// Oscillator noise mix.
    [Range(0.0f, 1.0f)]
    public float OscNoiseMix = 0.0f;

    /// Oscillator pitch shift.
    [Range(-4.0f, 4.0f)]
    public float OscPitchShift = 0.0f;

    /// Oscillator shape.
    [Range(0.0f, 1.0f)]
    public float OscShape = 0.0f;

    /// Oscillator skew.
    [Range(-0.5f, 0.5f)]
    public float OscSkew = 0.0f;

    [Header("Slice")]

    /// Slice mode.
    public SliceMode SliceMode = SliceMode.SUSTAIN;

    /// List of slices.
    public List<Slice> Slices = null;
    private int _sliceCount = 0;

    [Header("Filter")]

    /// Filter type.
    public FilterType FilterType = FilterType.NONE;

    /// Filter frequency in hertz.
    [Range(0.0f, 48000.0f)]
    public float FilterFrequency = 0.0f;

    /// Filter Q factor.
    [Range(0.1f, 10.0f)]
    public float FilterQ = 0.71f;

    [Header("Bit Crusher")]

    /// Bit crusher depth.
    [Range(1.0f, 16.0f)]
    public float BitCrusherDepth = 16.0f;

    /// Normalized bit crusher rate.
    [Range(0.0f, 1.0f)]
    public float BitCrusherRate = 1.0f;

    /// Note off callback.
    ///
    /// @param pitch Note pitch.
    public delegate void NoteOffCallback(float pitch);
    public event NoteOffCallback OnNoteOff;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public delegate void NoteOnCallback(float pitch, float intensity);
    public event NoteOnCallback OnNoteOn;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<float, float> {}
    public NoteOnEvent OnNoteOnEvent;

    /// Instrument create callback.
    public event Action OnInstrumentCreate;

    /// Instrument destroy callback.
    public event Action OnInstrumentDestroy;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param type Note control type
    /// @return Note control value.
    public float GetNoteControl(float pitch, NoteControlType type) {
      return Engine.Internal.Instrument_GetNoteControl(_handle, pitch, type);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(float pitch) {
      return Engine.Internal.Instrument_IsNoteOn(_handle, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Engine.Internal.Instrument_SetAllNotesOff(_handle);
    }

    /// Sets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param type Note control type.
    /// @param value Note control value.
    public void SetNoteControl(float pitch, NoteControlType type, float value) {
      Engine.Internal.Instrument_SetNoteControl(_handle, pitch, type, value);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(float pitch) {
      Engine.Internal.Instrument_SetNoteOff(_handle, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(float pitch, float intensity = 1.0f) {
      Engine.Internal.Instrument_SetNoteOn(_handle, pitch, intensity);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the handle.
      public static IntPtr GetHandle(Instrument instrument) {
        return instrument ? instrument._handle : IntPtr.Zero;
      }

      /// Internal note off callback.
      public static void OnNoteOff(Instrument instrument, float pitch) {
        instrument.OnNoteOff?.Invoke(pitch);
        instrument.OnNoteOffEvent?.Invoke(pitch);
      }

      /// Internal note on callback.
      public static void OnNoteOn(Instrument instrument, float pitch, float intensity) {
        instrument.OnNoteOn?.Invoke(pitch, intensity);
        instrument.OnNoteOnEvent?.Invoke(pitch, intensity);
      }
    }

    private void Awake() {
      Source = GetComponent<AudioSource>();
      Source.loop = true;
    }

    private void OnDestroy() {
      Source = null;
    }

    private void OnEnable() {
      Engine.Internal.Instrument_Create(this, ref _handle);
      OnInstrumentCreate?.Invoke();
      Source.clip = AudioClip.Create("[DO NOT EDIT]", 64, 1, AudioSettings.outputSampleRate, false);
      float[] ones = new float[64];
      for (int i = 0; i < ones.Length; ++i) {
        ones[i] = 1.0f;
      }
      Source.clip.SetData(ones, 0);
      Source.Play();
      Update();
    }

    private void OnDisable() {
      Source.Stop();
      Source.clip = null;
      OnInstrumentDestroy?.Invoke();
      Engine.Internal.Instrument_Destroy(ref _handle);
      _sliceCount = 0;
    }

    private void Update() {
      UpdateSampleData();
      SetControl(Engine.Internal.ControlType.GAIN, Gain);
      SetControl(Engine.Internal.ControlType.PITCH_SHIFT, PitchShift);
      SetControl(Engine.Internal.ControlType.RETRIGGER, Retrigger ? 1.0f : 0.0f);
      SetControl(Engine.Internal.ControlType.VOICE_COUNT, (float)VoiceCount);
      SetControl(Engine.Internal.ControlType.ATTACK, Attack);
      SetControl(Engine.Internal.ControlType.DECAY, Decay);
      SetControl(Engine.Internal.ControlType.SUSTAIN, Sustain);
      SetControl(Engine.Internal.ControlType.RELEASE, Release);
      SetControl(Engine.Internal.ControlType.OSC_MIX, OscMix);
      SetControl(Engine.Internal.ControlType.OSC_MODE, (float)OscMode);
      SetControl(Engine.Internal.ControlType.OSC_NOISE_MIX, OscNoiseMix);
      SetControl(Engine.Internal.ControlType.OSC_PITCH_SHIFT, OscPitchShift);
      SetControl(Engine.Internal.ControlType.OSC_SHAPE, OscShape);
      SetControl(Engine.Internal.ControlType.OSC_SKEW, OscSkew);
      SetControl(Engine.Internal.ControlType.SLICE_MODE, (float)SliceMode);
      SetControl(Engine.Internal.ControlType.FILTER_TYPE, (float)FilterType);
      SetControl(Engine.Internal.ControlType.FILTER_FREQUENCY, FilterFrequency);
      SetControl(Engine.Internal.ControlType.FILTER_TYPE, (float)FilterType);
      SetControl(Engine.Internal.ControlType.FILTER_FREQUENCY, FilterFrequency);
      SetControl(Engine.Internal.ControlType.FILTER_Q, FilterQ);
      SetControl(Engine.Internal.ControlType.BIT_CRUSHER_DEPTH, BitCrusherDepth);
      SetControl(Engine.Internal.ControlType.BIT_CRUSHER_RATE, BitCrusherRate);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Engine.Internal.Instrument_Process(_handle, data, channels);
    }

    private float GetControl(Engine.Internal.ControlType type) {
      return Engine.Internal.Instrument_GetControl(_handle, type);
    }

    private void SetControl(Engine.Internal.ControlType type, float value) {
      Engine.Internal.Instrument_SetControl(_handle, type, value);
    }

    private void UpdateSampleData() {
      if (Slices != null &&
          (Slices.Count != _sliceCount || Slices.Any(slice => slice.HasChanged))) {
        _sliceCount = Slices.Count;
        Engine.Internal.Instrument_SetSampleData(_handle, Slices);
      }
    }

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
