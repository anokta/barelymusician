using System;
using System.Collections.Generic;
using System.Linq;
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

  /// Control type.
  public enum ControlType {
    /// Gain.
    [InspectorName("Gain")] GAIN = 0,
    /// Number of voices.
    [InspectorName("Voice Count")] VOICE_COUNT,
    /// Oscillator type.
    [InspectorName("Oscillator Type")] OSCILLATOR_TYPE,
    /// Sample playback mode.
    [InspectorName("Sample Playback Mode")] SAMPLE_PLAYBACK_MODE,
    /// Envelope attack.
    [InspectorName("Attack")] ATTACK,
    /// Envelope decay.
    [InspectorName("Decay")] DECAY,
    /// Envelope sustain.
    [InspectorName("Sustain")] SUSTAIN,
    /// Envelope release.
    [InspectorName("Release")] RELEASE,
    /// Pitch shift.
    [InspectorName("Pitch Shift")] PITCH_SHIFT,
    /// Retrigger.
    [InspectorName("Retrigger")] RETRIGGER,
  }

  /// Note control type.
  public enum NoteControlType {
    /// Pitch shift.
    [InspectorName("Pitch Shift")] PITCH_SHIFT = 0,
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

    /// Sample playback mode.
    public SamplePlaybackMode SamplePlaybackMode = SamplePlaybackMode.NONE;

    /// List of samplers.
    public List<Sampler> Samplers = null;
    private int _samplerCount = 0;

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
    /// @param type Control type
    /// @return Control value.
    public double GetControl(ControlType type) {
      return Musician.Internal.Instrument_GetControl(_handle, type);
    }

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param type Note control type
    /// @return Note control value.
    public double GetNoteControl(double pitch, NoteControlType type) {
      return Musician.Internal.Instrument_GetNoteControl(_handle, pitch, type);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Instrument_IsNoteOn(_handle, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_handle);
    }

    /// Sets a control value.
    ///
    /// @param type Control type
    /// @param value Control value.
    public void SetControl(ControlType type, double value) {
      Musician.Internal.Instrument_SetControl(_handle, type, value);
    }

    /// Sets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param type Note control type.
    /// @param value Note control value.
    public void SetNoteControl(double pitch, NoteControlType type, double value) {
      Musician.Internal.Instrument_SetNoteControl(_handle, pitch, type, value);
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
      SetControl(ControlType.GAIN, Gain);
      SetControl(ControlType.VOICE_COUNT, (double)VoiceCount);
      SetControl(ControlType.OSCILLATOR_TYPE, (double)OscillatorType);
      SetControl(ControlType.SAMPLE_PLAYBACK_MODE, (double)SamplePlaybackMode);
      SetControl(ControlType.ATTACK, Attack);
      SetControl(ControlType.DECAY, Decay);
      SetControl(ControlType.SUSTAIN, Sustain);
      SetControl(ControlType.RELEASE, Release);
      SetControl(ControlType.PITCH_SHIFT, PitchShift);
      SetControl(ControlType.RETRIGGER, Retrigger ? 1.0 : 0.0);
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
