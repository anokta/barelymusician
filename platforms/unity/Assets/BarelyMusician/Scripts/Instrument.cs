using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Barely {
  /// Arpeggiator modes.
  public enum ArpMode {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// Up.
    [InspectorName("Up")] UP,
    /// Down.
    [InspectorName("Down")] DOWN,
    /// Random.
    [InspectorName("Random")] RANDOM,
  }

  /// Filter types.
  public enum FilterType {
    /// None.
    [InspectorName("None")] NONE = 0,
    /// Low-pass filter.
    [InspectorName("Lpf")] LPF,
    /// High-pass filter.
    [InspectorName("Hpf")] HPF,
  }

  /// Oscillator modes.
  public enum OscMode {
    /// Linear crossfade between the slice and the oscillator.
    [InspectorName("Crossfade")] CROSSFADE = 0,
    /// Amplitude modulation applied to the slice by the oscillator.
    [InspectorName("AM")] AM,
    /// Frequency modulation applied to the slice by the oscillator.
    [InspectorName("FM")] FM,
    /// Aamplitude modulation applied to the oscillator by the slice.
    [InspectorName("MA")] MA,
    /// Frequency modulation applied to the oscillator by the slice.
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

    /// Gain in logarithmic scale.
    [Range(0.0f, 1.0f)]
    public float Gain = 1.0f;

    /// Pitch shift.
    [Range(-2.0f, 2.0f)]
    public float PitchShift = 0.0f;

    [Range(-1.0f, 1.0f)]
    public float StereoPan = 0.0f;

    /// Retrigger.
    public bool Retrigger = false;

    /// Number of voices.
    [Range(1, 16)]
    public int VoiceCount = 8;

    [Header("Envelope")]

    /// Envelope attack in seconds.
    [Range(0.0f, 8.0f)]
    public float Attack = 0.05f;

    /// Envelope decay in seconds.
    [Range(0.0f, 8.0f)]
    public float Decay = 0.0f;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public float Sustain = 1.0f;

    /// Envelope release in seconds.
    [Range(0.0f, 8.0f)]
    public float Release = 0.25f;

    [Header("Oscillator")]

    /// Oscillator mix.
    [Range(0.0f, 1.0f)]
    public float OscMix = 0.5f;

    /// Oscillator mode.
    public OscMode OscMode = OscMode.CROSSFADE;

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
    [Range(-1.0f, 1.0f)]
    public float OscSkew = 0.0f;

    [Header("Slice")]

    /// Slice mode.
    public SliceMode SliceMode = SliceMode.SUSTAIN;

    /// List of slices.
    public List<Slice> Slices = null;
    private int _sliceCount = 0;

    [Header("Bit Crusher")]

    /// Bit crusher depth.
    [Range(0.0f, 1.0f)]
    public float BitCrusherDepth = 1.0f;

    /// Bit crusher rate.
    [Range(0.0f, 1.0f)]
    public float BitCrusherRate = 1.0f;

    [Header("Distortion")]

    /// Distortion mix.
    [Range(0.0f, 1.0f)]
    public float DistortionMix = 0.0f;

    /// Distortion drive.
    [Range(0.0f, 1.0f)]
    public float DistortionDrive = 0.0f;

    [Header("Filter")]

    /// Filter type.
    public FilterType FilterType = FilterType.NONE;

    /// Filter cutoff.
    [Range(0.0f, 1.0f)]
    public float FilterCutoff = 0.0f;

    /// Filter resonance.
    [Range(0.0f, 1.0f)]
    public float FilterResonance = 0.5f;

    [Header("Send Effects")]

    /// Delay send.
    [Range(0.0f, 1.0f)]
    public float DelaySend = 0.0f;

    /// Reverb send.
    [Range(0.0f, 2.0f)]
    public float ReverbSend = 0.0f;

    /// Sidechain send.
    [Range(-1.0f, 1.0f)]
    public float SidechainSend = 0.0f;

    [Header("Arpeggiator")]

    /// Arpeggiator mode.
    public ArpMode ArpMode = ArpMode.NONE;

    /// Arpeggiator gate ratio.
    [Range(0.001f, 1.0f)]
    public float ArpGate = 0.5f;

    /// Arpeggiator rate.
    [Range(0.001f, 16.0f)]
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
    /// @return Note gain in logarithmic scale.
    public float GetNoteGain(float pitch) {
      return Engine.Internal.Instrument_GetNoteControl(_id, pitch,
                                                       Engine.Internal.NoteControlType.GAIN);
    }

    /// Returns the pitch shift of a note.
    ///
    /// @param pitch Note pitch.
    /// @return Pitch shift.
    public float GetNotePitchShift(float pitch) {
      return Engine.Internal.Instrument_GetNoteControl(_id, pitch,
                                                       Engine.Internal.NoteControlType.PITCH_SHIFT);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(float pitch) {
      return Engine.Internal.Instrument_IsNoteOn(_id, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Engine.Internal.Instrument_SetAllNotesOff(_id);
    }

    /// Sets the gain of a note.
    ///
    /// @param pitch Note pitch.
    /// @param gain Gain in logarithmic scale.
    public void SetNoteGain(float pitch, float gain) {
      Engine.Internal.Instrument_SetNoteControl(_id, pitch, Engine.Internal.NoteControlType.GAIN,
                                                gain);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(float pitch) {
      Engine.Internal.Instrument_SetNoteOff(_id, pitch);
    }

    /// Sets the pitch shift of a note.
    ///
    /// @param pitch Note pitch.
    /// @param pitchShift Note pitch shift.
    public void SetNotePitchShift(float pitch, float pitchShift) {
      Engine.Internal.Instrument_SetNoteControl(
          _id, pitch, Engine.Internal.NoteControlType.PITCH_SHIFT, pitchShift);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param gain Note gain.
    /// @param pitchShift Note pitch shift.
    public void SetNoteOn(float pitch, float gain = 1.0f, float pitchShift = 0.0f) {
      Engine.Internal.Instrument_SetNoteOn(_id, pitch, gain, pitchShift);
    }

    public static class Internal {
      public static void OnNoteOff(Instrument instrument, float pitch) {
        instrument.OnNoteOff?.Invoke(pitch);
        instrument.OnNoteOffEvent?.Invoke(pitch);
      }
      public static void OnNoteOn(Instrument instrument, float pitch) {
        instrument.OnNoteOn?.Invoke(pitch);
        instrument.OnNoteOnEvent?.Invoke(pitch);
      }
    }

    private void OnEnable() {
      Engine.Internal.Instrument_Create(this, ref _id);
      Update();
    }

    private void OnDisable() {
      Engine.Internal.Instrument_Destroy(ref _id);
      _sliceCount = 0;
    }

    private void Update() {
      UpdateSampleData();
      SetControl(Engine.Internal.InstrumentControlType.GAIN, Gain);
      SetControl(Engine.Internal.InstrumentControlType.PITCH_SHIFT, PitchShift);
      SetControl(Engine.Internal.InstrumentControlType.STEREO_PAN, StereoPan);
      SetControl(Engine.Internal.InstrumentControlType.RETRIGGER, Retrigger ? 1.0f : 0.0f);
      SetControl(Engine.Internal.InstrumentControlType.VOICE_COUNT, (float)VoiceCount);
      SetControl(Engine.Internal.InstrumentControlType.ATTACK, Attack);
      SetControl(Engine.Internal.InstrumentControlType.DECAY, Decay);
      SetControl(Engine.Internal.InstrumentControlType.SUSTAIN, Sustain);
      SetControl(Engine.Internal.InstrumentControlType.RELEASE, Release);
      SetControl(Engine.Internal.InstrumentControlType.OSC_MIX, OscMix);
      SetControl(Engine.Internal.InstrumentControlType.OSC_MODE, (float)OscMode);
      SetControl(Engine.Internal.InstrumentControlType.OSC_NOISE_MIX, OscNoiseMix);
      SetControl(Engine.Internal.InstrumentControlType.OSC_PITCH_SHIFT, OscPitchShift);
      SetControl(Engine.Internal.InstrumentControlType.OSC_SHAPE, OscShape);
      SetControl(Engine.Internal.InstrumentControlType.OSC_SKEW, OscSkew);
      SetControl(Engine.Internal.InstrumentControlType.SLICE_MODE, (float)SliceMode);
      SetControl(Engine.Internal.InstrumentControlType.BIT_CRUSHER_DEPTH, BitCrusherDepth);
      SetControl(Engine.Internal.InstrumentControlType.BIT_CRUSHER_RATE, BitCrusherRate);
      SetControl(Engine.Internal.InstrumentControlType.DISTORTION_MIX, DistortionMix);
      SetControl(Engine.Internal.InstrumentControlType.DISTORTION_DRIVE, DistortionDrive);
      SetControl(Engine.Internal.InstrumentControlType.FILTER_TYPE, (float)FilterType);
      SetControl(Engine.Internal.InstrumentControlType.FILTER_CUTOFF, FilterCutoff);
      SetControl(Engine.Internal.InstrumentControlType.FILTER_RESONANCE, FilterResonance);
      SetControl(Engine.Internal.InstrumentControlType.DELAY_SEND, DelaySend);
      SetControl(Engine.Internal.InstrumentControlType.REVERB_SEND, ReverbSend);
      SetControl(Engine.Internal.InstrumentControlType.SIDECHAIN_SEND, SidechainSend);
      SetControl(Engine.Internal.InstrumentControlType.ARP_MODE, (float)ArpMode);
      SetControl(Engine.Internal.InstrumentControlType.ARP_GATE, ArpGate);
      SetControl(Engine.Internal.InstrumentControlType.ARP_RATE, ArpRate);
    }

    private void SetControl(Engine.Internal.InstrumentControlType type, float value) {
      Engine.Internal.Instrument_SetControl(_id, type, value);
    }

    private void UpdateSampleData() {
      if (Slices != null &&
          (Slices.Count != _sliceCount || Slices.Any(slice => slice.HasChanged))) {
        _sliceCount = Slices.Count;
        Engine.Internal.Instrument_SetSampleData(_id, Slices);
      }
    }

    private UInt32 _id = 0;
  }
}  // namespace Barely
