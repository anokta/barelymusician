using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Barely {
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
      public double[] Data {
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

    /// Gain in logarithmic scale.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Pitch shift.
    [Range(-2.0f, 2.0f)]
    public double PitchShift = 0.0;

    [Range(-1.0f, 1.0f)]
    public double StereoPan = 0.0;

    [Header("Envelope")]

    /// Envelope attack in seconds.
    [Range(0.0f, 8.0f)]
    public double Attack = 0.05;

    /// Envelope decay in seconds.
    [Range(0.0f, 8.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release in seconds.
    [Range(0.0f, 8.0f)]
    public double Release = 0.25;

    [Header("Slice")]

    /// Slice playback mode.
    public SliceMode SliceMode = SliceMode.SUSTAIN;

    /// List of slices.
    public List<Slice> Slices = null;
    private int _sliceCount = 0;

    [Header("Oscillator")]

    /// Oscillator mix.
    [Range(0.0f, 1.0f)]
    public double OscMix = 0.5;

    /// Oscillator mode.
    public OscMode OscMode = OscMode.CROSSFADE;

    /// Oscillator noise mix.
    [Range(0.0f, 1.0f)]
    public double OscNoiseMix = 0.0;

    /// Oscillator pitch shift.
    [Range(-2.0f, 2.0f)]
    public double OscPitchShift = 0.0;

    /// Oscillator shape.
    [Range(0.0f, 1.0f)]
    public double OscShape = 0.0;

    /// Oscillator skew.
    [Range(-1.0f, 1.0f)]
    public double OscSkew = 0.0;

    [Header("Bit Crusher")]

    /// Bit crusher depth.
    [Range(0.0f, 1.0f)]
    public double CrushDepth = 0.0;

    /// Bit crusher rate.
    [Range(0.0f, 1.0f)]
    public double CrushRate = 0.0;

    [Header("Distortion")]

    /// Distortion mix.
    [Range(0.0f, 1.0f)]
    public double DistortionMix = 0.0;

    /// Distortion drive.
    [Range(0.0f, 1.0f)]
    public double DistortionDrive = 0.0;

    [Header("Filter")]

    /// Filter cutoff.
    [Range(0.0f, 1.0f)]
    public double FilterCutoff = 1.0;

    /// Filter resonance.
    [Range(0.0f, 1.0f)]
    public double FilterResonance = 0.5;

    /// Filter tone.
    [Range(-1.0f, 1.0f)]
    public double FilterTone = 0.0;

    [Header("Send Effects")]

    /// Delay send.
    [Range(0.0f, 1.0f)]
    public double DelaySend = 0.0;

    /// Reverb send.
    [Range(0.0f, 2.0f)]
    public double ReverbSend = 0.0;

    /// Sidechain send.
    [Range(-1.0f, 1.0f)]
    public double SidechainSend = 0.0;

    [Header("Polyphony")]

    /// Retrigger.
    public bool Retrigger = false;

    /// Number of voices.
    [Range(1, 16)]
    public int VoiceCount = 8;

    /// Note off callback.
    /// @param pitch Note pitch.
    public delegate void NoteOffCallback(double pitch);
    public event NoteOffCallback OnNoteOff;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<double> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on callback.
    /// @param pitch Note pitch.
    public delegate void NoteOnCallback(double pitch);
    public event NoteOnCallback OnNoteOn;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<double> {}
    public NoteOnEvent OnNoteOnEvent;

    /// Set of active note pitches.
    public HashSet<double> Pitches { get; private set; } = new HashSet<double>();

    /// Returns whether a note is on or not.
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Pitches.Contains(pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      HashSet<double> pitches = Pitches;
      Pitches = new HashSet<double>();
      foreach (double pitch in pitches) {
        Engine.Internal.Instrument_SetNoteOff(_id, pitch);
        OnNoteOff?.Invoke(pitch);
        OnNoteOffEvent?.Invoke(pitch);
      }
    }

    /// Sets the gain of a note.
    /// @param pitch Note pitch.
    /// @param gain Note gain.
    public void SetNoteGain(double pitch, double gain) {
      Engine.Internal.Instrument_SetNoteControl(_id, pitch, Engine.Internal.NoteControlType.GAIN,
                                                gain);
    }

    /// Sets a note off.
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      if (Pitches.Remove(pitch)) {
        Engine.Internal.Instrument_SetNoteOff(_id, pitch);
        OnNoteOff?.Invoke(pitch);
        OnNoteOffEvent?.Invoke(pitch);
      }
    }

    /// Sets the pitch shift of a note.
    /// @param pitch Note pitch.
    /// @param pitchShift Note pitch shift.
    public void SetNotePitchShift(double pitch, double pitchShift) {
      Engine.Internal.Instrument_SetNoteControl(
          _id, pitch, Engine.Internal.NoteControlType.PITCH_SHIFT, pitchShift);
    }

    /// Sets a note on.
    /// @param pitch Note pitch.
    /// @param gain Note gain.
    /// @param pitchShift Note pitch shift.
    public void SetNoteOn(double pitch, double gain = 1.0, double pitchShift = 0.0) {
      if (Pitches.Add(pitch)) {
        Engine.Internal.Instrument_SetNoteOn(_id, pitch);
        if (gain != 1.0) {
          SetNoteGain(pitch, gain);
        }
        if (pitchShift != 0.0) {
          SetNotePitchShift(pitch, pitchShift);
        }
        OnNoteOn?.Invoke(pitch);
        OnNoteOnEvent?.Invoke(pitch);
      }
    }

    private void OnEnable() {
      Array.Fill(_controls, double.NaN);
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
      SetControl(Engine.Internal.InstrumentControlType.ATTACK, Attack);
      SetControl(Engine.Internal.InstrumentControlType.DECAY, Decay);
      SetControl(Engine.Internal.InstrumentControlType.SUSTAIN, Sustain);
      SetControl(Engine.Internal.InstrumentControlType.RELEASE, Release);
      SetControl(Engine.Internal.InstrumentControlType.SLICE_MODE, (double)SliceMode);
      SetControl(Engine.Internal.InstrumentControlType.OSC_MIX, OscMix);
      SetControl(Engine.Internal.InstrumentControlType.OSC_MODE, (double)OscMode);
      SetControl(Engine.Internal.InstrumentControlType.OSC_NOISE_MIX, OscNoiseMix);
      SetControl(Engine.Internal.InstrumentControlType.OSC_PITCH_SHIFT, OscPitchShift);
      SetControl(Engine.Internal.InstrumentControlType.OSC_SHAPE, OscShape);
      SetControl(Engine.Internal.InstrumentControlType.OSC_SKEW, OscSkew);
      SetControl(Engine.Internal.InstrumentControlType.CRUSH_DEPTH, CrushDepth);
      SetControl(Engine.Internal.InstrumentControlType.CRUSH_RATE, CrushRate);
      SetControl(Engine.Internal.InstrumentControlType.DISTORTION_MIX, DistortionMix);
      SetControl(Engine.Internal.InstrumentControlType.DISTORTION_DRIVE, DistortionDrive);
      SetControl(Engine.Internal.InstrumentControlType.FILTER_CUTOFF, FilterCutoff);
      SetControl(Engine.Internal.InstrumentControlType.FILTER_RESONANCE, FilterResonance);
      SetControl(Engine.Internal.InstrumentControlType.FILTER_TONE, FilterTone);
      SetControl(Engine.Internal.InstrumentControlType.DELAY_SEND, DelaySend);
      SetControl(Engine.Internal.InstrumentControlType.REVERB_SEND, ReverbSend);
      SetControl(Engine.Internal.InstrumentControlType.SIDECHAIN_SEND, SidechainSend);
      SetControl(Engine.Internal.InstrumentControlType.RETRIGGER, Retrigger ? 1.0 : 0.0);
      SetControl(Engine.Internal.InstrumentControlType.VOICE_COUNT, (double)VoiceCount);
    }

    private void SetControl(Engine.Internal.InstrumentControlType type, double value) {
      if (_controls[(int)type] != value) {
        Engine.Internal.Instrument_SetControl(_id, type, value);
        _controls[(int)type] = value;
      }
    }

    private void UpdateSampleData() {
      if (Slices != null &&
          (Slices.Count != _sliceCount || Slices.Any(slice => slice.HasChanged))) {
        _sliceCount = Slices.Count;
        Engine.Internal.Instrument_SetSampleData(_id, Slices);
      }
    }

    private double[] _controls = new double[(int)Engine.Internal.InstrumentControlType.COUNT];
    private UInt32 _id = 0;
  }
}  // namespace Barely
