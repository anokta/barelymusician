﻿using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Barely {
  /// Filter type.
  public enum FilterType {
    /// None.
    [InspectorName("None")] NONE,
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
  public enum OscillatorMode {
    /// Mix.
    [InspectorName("Mix")] MIX = 0,
    /// Amplitude modulation.
    [InspectorName("AM")] AM,
    /// Envelope follower.
    [InspectorName("Envlope Follower")] ENVELOPE_FOLLOWER,
    /// Ring modulation.
    [InspectorName("Ring")] RING,
  }

  /// Oscillator shape.
  public enum OscillatorShape {
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

  /// Sample playback mode.
  public enum SamplePlaybackMode {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// Once.
    [InspectorName("Once")] ONCE,
    /// Sustain.
    [InspectorName("Sustain")] SUSTAIN,
    /// Loop.
    [InspectorName("Loop")] LOOP,
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
    [Range(-1.0f, 1.0f)]
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
    public OscillatorMode OscillatorMode = OscillatorMode.MIX;

    /// Oscillator mix.
    [Range(-1.0f, 1.0f)]
    public float OscillatorMix = 0.0f;

    /// Oscillator pitch shift.
    [Range(-1.0f, 1.0f)]
    public float OscillatorPitchShift = 0.0f;

    /// Oscillator shape.
    public OscillatorShape OscillatorShape = OscillatorShape.NONE;

    /// Pulse width.
    [Range(0.0f, 1.0f)]
    public float PulseWidth = 0.5f;

    [Header("Sample Player")]

    /// Sample playback mode.
    public SamplePlaybackMode SamplePlaybackMode = SamplePlaybackMode.NONE;

    /// List of sample data slices.
    public List<Slice> Slices = null;
    private int _sliceCount = 0;

    [Header("Filter")]

    /// Filter type.
    public FilterType FilterType = FilterType.NONE;

    /// Filter frequency in hertz.
    [Range(0.0f, 48000.0f)]
    public float FilterFrequency = 0.0f;

    /// Note off event callback.
    ///
    /// @param pitch Note pitch.
    public delegate void NoteOffEventCallback(float pitch);
    public event NoteOffEventCallback OnNoteOff;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on event callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public delegate void NoteOnEventCallback(float pitch, float intensity);
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

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param type Note control type
    /// @return Note control value.
    public float GetNoteControl(float pitch, NoteControlType type) {
      return Musician.Internal.Instrument_GetNoteControl(_handle, pitch, type);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(float pitch) {
      return Musician.Internal.Instrument_IsNoteOn(_handle, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_handle);
    }

    /// Sets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param type Note control type.
    /// @param value Note control value.
    public void SetNoteControl(float pitch, NoteControlType type, float value) {
      Musician.Internal.Instrument_SetNoteControl(_handle, pitch, type, value);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(float pitch) {
      Musician.Internal.Instrument_SetNoteOff(_handle, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(float pitch, float intensity = 1.0f) {
      Musician.Internal.Instrument_SetNoteOn(_handle, pitch, intensity);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the handle.
      public static IntPtr GetHandle(Instrument instrument) {
        return instrument ? instrument._handle : IntPtr.Zero;
      }

      /// Internal note off event callback.
      public static void OnNoteOffEvent(Instrument instrument, float pitch) {
        instrument.OnNoteOff?.Invoke(pitch);
        instrument.OnNoteOffEvent?.Invoke(pitch);
      }

      /// Internal note on event callback.
      public static void OnNoteOnEvent(Instrument instrument, float pitch, float intensity) {
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
      Musician.Internal.Instrument_Create(this, ref _handle);
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
      Musician.Internal.Instrument_Destroy(ref _handle);
      _sliceCount = 0;
    }

    private void Update() {
      UpdateSampleData();
      SetControl(Musician.Internal.ControlType.GAIN, Gain);
      SetControl(Musician.Internal.ControlType.PITCH_SHIFT, PitchShift);
      SetControl(Musician.Internal.ControlType.RETRIGGER, Retrigger ? 1.0f : 0.0f);
      SetControl(Musician.Internal.ControlType.VOICE_COUNT, (float)VoiceCount);
      SetControl(Musician.Internal.ControlType.ATTACK, Attack);
      SetControl(Musician.Internal.ControlType.DECAY, Decay);
      SetControl(Musician.Internal.ControlType.SUSTAIN, Sustain);
      SetControl(Musician.Internal.ControlType.RELEASE, Release);
      SetControl(Musician.Internal.ControlType.OSCILLATOR_MIX, OscillatorMix);
      SetControl(Musician.Internal.ControlType.OSCILLATOR_MODE, (float)OscillatorMode);
      SetControl(Musician.Internal.ControlType.OSCILLATOR_PITCH_SHIFT, OscillatorPitchShift);
      SetControl(Musician.Internal.ControlType.OSCILLATOR_SHAPE, (float)OscillatorShape);
      SetControl(Musician.Internal.ControlType.PULSE_WIDTH, PulseWidth);
      SetControl(Musician.Internal.ControlType.SAMPLE_PLAYBACK_MODE, (float)SamplePlaybackMode);
      SetControl(Musician.Internal.ControlType.FILTER_TYPE, (float)FilterType);
      SetControl(Musician.Internal.ControlType.FILTER_FREQUENCY, FilterFrequency);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_handle, data, channels);
    }

    private float GetControl(Musician.Internal.ControlType type) {
      return Musician.Internal.Instrument_GetControl(_handle, type);
    }

    private void SetControl(Musician.Internal.ControlType type, float value) {
      Musician.Internal.Instrument_SetControl(_handle, type, value);
    }

    private void UpdateSampleData() {
      if (Slices != null &&
          (Slices.Count != _sliceCount || Slices.Any(slice => slice.HasChanged))) {
        _sliceCount = Slices.Count;
        Musician.Internal.Instrument_SetSampleData(_handle, Slices);
      }
    }

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
