using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Barely {
  /// Arpeggiator modes.
  public enum ArpMode {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// None.
    [InspectorName("Up")] UP,
    /// None.
    [InspectorName("Down")] DOWN,
    /// None.
    [InspectorName("Random")] RANDOM,
  }

  /// Filter types.
  public enum FilterType {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// Low-pass.
    [InspectorName("Low-pass")] LOW_PASS,
    /// High-pass.
    [InspectorName("High-pass")] HIGH_PASS,
  }

  /// Oscillator modes.
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

  /// Slice modes.
  public enum SliceMode {
    /// Sustain.
    [InspectorName("Sustain")] SUSTAIN = 0,
    /// Loop.
    [InspectorName("Loop")] LOOP,
    /// Once.
    [InspectorName("Once")] ONCE,
  }

  /// A representation of a musical instrument that can be played in real-time.
  [ExecuteInEditMode]
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
            if (Sample != null && Sample.loadState != AudioDataLoadState.Loaded) {
              return null;  // not ready yet.
            }
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

    /// Gain in linear amplitude.
    [Range(0.0f, 1.0f)]
    public float Gain = 1.0f;

    /// Pitch shift.
    [Range(-2.0f, 2.0f)]
    public float PitchShift = 0.0f;

    /// Retrigger.
    public bool Retrigger = false;

    [Range(-1.0f, 1.0f)]
    public float StereoPan = 0.0f;

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
    public float OscMix = 0.5f;

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

    [Header("Send Effects")]

    /// Delay send.
    [Range(0.0f, 1.0f)]
    public float DelaySend = 0.0f;

    /// Sidechain send.
    [Range(-1.0f, 1.0f)]
    public float SidechainSend = 0.0f;

    [Header("Arpeggiator")]

    /// Arpeggiator mode.
    public ArpMode ArpMode = ArpMode.NONE;

    /// Arpeggiator gate ratio.
    [Range(0.0f, 1.0f)]
    public float ArpGateRatio = 0.5f;

    /// Arpeggiator rate.
    [Range(0.0f, 16.0f)]
    public float ArpRate = 1.0f;

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
    public delegate void NoteOnCallback(float pitch);
    public event NoteOnCallback OnNoteOn;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOnEvent OnNoteOnEvent;

    /// Returns the gain of a note.
    ///
    /// @param pitch Note pitch.
    /// @return Note gain in linear amplitude.
    public float GetNoteGain(float pitch) {
      return Engine.Internal.Instrument_GetNoteControl(_handle, pitch,
                                                       Engine.Internal.NoteControlType.GAIN);
    }

    /// Returns the pitch shift of a note.
    ///
    /// @param pitch Note pitch.
    /// @return Pitch shift.
    public float GetNotePitchShift(float pitch) {
      return Engine.Internal.Instrument_GetNoteControl(_handle, pitch,
                                                       Engine.Internal.NoteControlType.PITCH_SHIFT);
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

    /// Sets the gain of a note.
    ///
    /// @param pitch Note pitch.
    /// @param gain Gain in linear amplitude.
    public void SetNoteGain(float pitch, float gain) {
      Engine.Internal.Instrument_SetNoteControl(_handle, pitch,
                                                Engine.Internal.NoteControlType.GAIN, gain);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(float pitch) {
      Engine.Internal.Instrument_SetNoteOff(_handle, pitch);
    }

    /// Sets the pitch shift of a note.
    ///
    /// @param pitch Note pitch.
    /// @param pitchShift Note pitch shift.
    public void SetNotePitchShift(float pitch, float pitchShift) {
      Engine.Internal.Instrument_SetNoteControl(
          _handle, pitch, Engine.Internal.NoteControlType.PITCH_SHIFT, pitchShift);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param gain Note gain.
    /// @param pitchShift Note pitch shift.
    public void SetNoteOn(float pitch, float gain = 1.0f, float pitchShift = 0.0f) {
      Engine.Internal.Instrument_SetNoteOn(_handle, pitch, gain, pitchShift);
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
      public static void OnNoteOn(Instrument instrument, float pitch) {
        instrument.OnNoteOn?.Invoke(pitch);
        instrument.OnNoteOnEvent?.Invoke(pitch);
      }
    }

    private void OnEnable() {
      Engine.Internal.Instrument_Create(this, ref _handle);
      Update();
    }

    private void OnDisable() {
      Engine.Internal.Instrument_Destroy(ref _handle);
      _sliceCount = 0;
    }

    private void Update() {
      UpdateSampleData();
      SetControl(Engine.Internal.ControlType.GAIN, Gain);
      SetControl(Engine.Internal.ControlType.PITCH_SHIFT, PitchShift);
      SetControl(Engine.Internal.ControlType.RETRIGGER, Retrigger ? 1.0f : 0.0f);
      SetControl(Engine.Internal.ControlType.STEREO_PAN, StereoPan);
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
      SetControl(Engine.Internal.ControlType.DELAY_SEND, DelaySend);
      SetControl(Engine.Internal.ControlType.SIDECHAIN_SEND, SidechainSend);
      SetControl(Engine.Internal.ControlType.ARP_MODE, (float)ArpMode);
      SetControl(Engine.Internal.ControlType.ARP_GATE_RATIO, ArpGateRatio);
      SetControl(Engine.Internal.ControlType.ARP_RATE, ArpRate);
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
