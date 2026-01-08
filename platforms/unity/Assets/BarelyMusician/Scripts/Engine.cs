using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Xml.Serialization;
using UnityEngine;

namespace Barely {
  /// A representation of an engine that governs all musical components.
  public static class Engine {
    /// Compressor mix.
    public static float CompressorMix {
      get { return _compressorMix; }
      set {
        if (_compressorMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMPRESSOR_MIX, value);
          _compressorMix = Internal.Engine_GetControl(Internal.EngineControlType.COMPRESSOR_MIX);
        }
      }
    }
    private static float _compressorMix = 1.0f;

    /// Compressor attack.
    public static float CompressorAttack {
      get { return _compressorAttack; }
      set {
        if (_compressorAttack != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMPRESSOR_ATTACK, value);
          _compressorAttack =
              Internal.Engine_GetControl(Internal.EngineControlType.COMPRESSOR_ATTACK);
        }
      }
    }
    private static float _compressorAttack = 0.0f;

    /// Compressor release.
    public static float CompressorRelease {
      get { return _compressorRelease; }
      set {
        if (_compressorRelease != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMPRESSOR_RELEASE, value);
          _compressorRelease =
              Internal.Engine_GetControl(Internal.EngineControlType.COMPRESSOR_RELEASE);
        }
      }
    }
    private static float _compressorRelease = 0.0f;

    /// Compressor threshold.
    public static float CompressorThreshold {
      get { return _compressorThreshold; }
      set {
        if (_compressorThreshold != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMPRESSOR_THRESHOLD, value);
          _compressorThreshold =
              Internal.Engine_GetControl(Internal.EngineControlType.COMPRESSOR_THRESHOLD);
        }
      }
    }
    private static float _compressorThreshold = 1.0f;

    /// Compressor ratio.
    public static float CompressorRatio {
      get { return _compressorRatio; }
      set {
        if (_compressorRatio != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMPRESSOR_RATIO, value);
          _compressorRatio =
              Internal.Engine_GetControl(Internal.EngineControlType.COMPRESSOR_RATIO);
        }
      }
    }
    private static float _compressorRatio = 1.0f;

    /// Delay mix.
    public static float DelayMix {
      get { return _delayMix; }
      set {
        if (_delayMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_MIX, value);
          _delayMix = Internal.Engine_GetControl(Internal.EngineControlType.DELAY_MIX);
        }
      }
    }
    private static float _delayMix = 1.0f;

    /// Delay time in seconds.
    public static float DelayTime {
      get { return _delayTime; }
      set {
        if (_delayTime != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_TIME, value);
          _delayTime = Internal.Engine_GetControl(Internal.EngineControlType.DELAY_TIME);
        }
      }
    }
    private static float _delayTime = 0.0f;

    /// Delay feedback.
    public static float DelayFeedback {
      get { return _delayFeedback; }
      set {
        if (_delayFeedback != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_FEEDBACK, value);
          _delayFeedback = Internal.Engine_GetControl(Internal.EngineControlType.DELAY_FEEDBACK);
        }
      }
    }
    private static float _delayFeedback = 0.0f;

    /// Delay low-pass frequency.
    public static float DelayLowPassFrequency {
      get { return _delayLowPassFrequency; }
      set {
        if (_delayLowPassFrequency != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_LOW_PASS_FREQUENCY, value);
          _delayLowPassFrequency =
              Internal.Engine_GetControl(Internal.EngineControlType.DELAY_LOW_PASS_FREQUENCY);
        }
      }
    }
    private static float _delayLowPassFrequency = 48000.0f;

    /// Delay high-pass frequency.
    public static float DelayHighPassFrequency {
      get { return _delayHighPassFrequency; }
      set {
        if (_delayHighPassFrequency != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_HIGH_PASS_FREQUENCY, value);
          _delayHighPassFrequency =
              Internal.Engine_GetControl(Internal.EngineControlType.DELAY_HIGH_PASS_FREQUENCY);
        }
      }
    }
    private static float _delayHighPassFrequency = 0.0f;

    /// Sidechain mix.
    public static float SidechainMix {
      get { return _sidechainMix; }
      set {
        if (_sidechainMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_MIX, value);
          _sidechainMix = Internal.Engine_GetControl(Internal.EngineControlType.SIDECHAIN_MIX);
        }
      }
    }
    private static float _sidechainMix = 1.0f;

    /// Sidechain attack.
    public static float SidechainAttack {
      get { return _sidechainAttack; }
      set {
        if (_sidechainAttack != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_ATTACK, value);
          _sidechainAttack =
              Internal.Engine_GetControl(Internal.EngineControlType.SIDECHAIN_ATTACK);
        }
      }
    }
    private static float _sidechainAttack = 0.0f;

    /// Sidechain release.
    public static float SidechainRelease {
      get { return _sidechainRelease; }
      set {
        if (_sidechainRelease != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_RELEASE, value);
          _sidechainRelease =
              Internal.Engine_GetControl(Internal.EngineControlType.SIDECHAIN_RELEASE);
        }
      }
    }
    private static float _sidechainRelease = 0.0f;

    /// Sidechain threshold.
    public static float SidechainThreshold {
      get { return _sidechainThreshold; }
      set {
        if (_sidechainThreshold != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_THRESHOLD, value);
          _sidechainThreshold =
              Internal.Engine_GetControl(Internal.EngineControlType.SIDECHAIN_THRESHOLD);
        }
      }
    }
    private static float _sidechainThreshold = 1.0f;

    /// Sidechain ratio.
    public static float SidechainRatio {
      get { return _sidechainRatio; }
      set {
        if (_sidechainRatio != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_RATIO, value);
          _sidechainRatio = Internal.Engine_GetControl(Internal.EngineControlType.SIDECHAIN_RATIO);
        }
      }
    }
    private static float _sidechainRatio = 1.0f;

    /// Tempo in beats per minute.
    public static double Tempo {
      get { return _tempo; }
      set {
        if (_tempo != value) {
          Internal.Engine_SetTempo(value);
          _tempo = Internal.Engine_GetTempo();
        }
      }
    }
    private static double _tempo = 120.0;

    /// Timestamp in seconds.
    public static double Timestamp {
      get { return Internal.Engine_GetTimestamp(); }
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Instrument control type.
      public enum InstrumentControlType {
        /// Gain in linear amplitude.
        [InspectorName("Gain")] GAIN = 0,
        /// Pitch shift.
        [InspectorName("Pitch Shift")] PITCH_SHIFT,
        /// Retrigger.
        [InspectorName("Retrigger")] RETRIGGER,
        /// Stereo pan.
        [InspectorName("Stereo Pan")] STEREO_PAN,
        /// Number of voices.
        [InspectorName("Voice Count")] VOICE_COUNT,
        /// Envelope attack in seconds.
        [InspectorName("Attack")] ATTACK,
        /// Envelope decay in seconds.
        [InspectorName("Decay")] DECAY,
        /// Envelope sustain.
        [InspectorName("Sustain")] SUSTAIN,
        /// Envelope release in seconds.
        [InspectorName("Release")] RELEASE,
        /// Oscillator mix.
        [InspectorName("Oscillator Mix")] OSC_MIX,
        /// Oscillator mode.
        [InspectorName("Oscillator Mode")] OSC_MODE,
        /// Oscillator noise mix.
        [InspectorName("Oscillator Noise Mix")] OSC_NOISE_MIX,
        /// Oscillator pitch shift.
        [InspectorName("Oscillator Pitch Shift")] OSC_PITCH_SHIFT,
        /// Oscillator shape.
        [InspectorName("Oscillator Shape")] OSC_SHAPE,
        /// Oscillator skew.
        [InspectorName("Oscillator Skew")] OSC_SKEW,
        /// Slice mode.
        [InspectorName("Slice Mode")] SLICE_MODE,
        /// Bit crusher depth.
        [InspectorName("Bit Crusher Depth")] BIT_CRUSHER_DEPTH,
        /// Bit crusher rate.
        [InspectorName("Bit Crusher Rate")] BIT_CRUSHER_RATE,
        /// Distortion amount.
        [InspectorName("Distortion Amount")] DISTORTION_AMOUNT,
        /// Distortion drive.
        [InspectorName("Distortion Drive")] DISTORTION_DRIVE,
        /// Filter type.
        [InspectorName("Filter Type")] FILTER_TYPE,
        /// Filter frequency in hertz.
        [InspectorName("Filter Frequency")] FILTER_FREQUENCY,
        /// Filter Q factor.
        [InspectorName("Filter Q")] FILTER_Q,
        /// Delay send.
        [InspectorName("Delay Send")] DELAY_SEND,
        /// Sidechain send.
        [InspectorName("Sidechain Send")] SIDECHAIN_SEND,
        /// Arpeggiator mode.
        [InspectorName("Arpeggiator Mode")] ARP_MODE,
        /// Arpeggiator gate ratio.
        [InspectorName("Arpeggiator Gate Ratio")] ARP_GATE_RATIO,
        /// Arpeggiator rate.
        [InspectorName("Arpeggiator Rate")] ARP_RATE,
      }

      /// Engine control type.
      public enum EngineControlType {
        /// Compressor mix.
        [InspectorName("Compressor Mix")] COMPRESSOR_MIX = 0,
        /// Compressor attack in seconds.
        [InspectorName("Compressor Attack")] COMPRESSOR_ATTACK,
        /// Compressor release in seconds.
        [InspectorName("Compressor Release")] COMPRESSOR_RELEASE,
        /// Compressor threshold.
        [InspectorName("Compressor Threshold")] COMPRESSOR_THRESHOLD,
        /// Compressor ratio.
        [InspectorName("Compressor Ratio")] COMPRESSOR_RATIO,
        /// Delay mix.
        [InspectorName("Delay Mix")] DELAY_MIX,
        /// Delay time in seconds.
        [InspectorName("Delay Time")] DELAY_TIME,
        /// Delay feedback.
        [InspectorName("Delay Feedback")] DELAY_FEEDBACK,
        /// Delay low-pass frequency.
        [InspectorName("Delay Low-Pass Frequency")] DELAY_LOW_PASS_FREQUENCY,
        /// Delay high-pass frequency.
        [InspectorName("Delay High-Pass Frequency")] DELAY_HIGH_PASS_FREQUENCY,
        /// Sidechain mix.
        [InspectorName("Sidechain Mix")] SIDECHAIN_MIX,
        /// Sidechain attack in seconds.
        [InspectorName("Sidechain Attack")] SIDECHAIN_ATTACK,
        /// Sidechain release in seconds.
        [InspectorName("Sidechain Release")] SIDECHAIN_RELEASE,
        /// Sidechain threshold.
        [InspectorName("Sidechain Threshold")] SIDECHAIN_THRESHOLD,
        /// Sidechain ratio.
        [InspectorName("Sidechain Ratio")] SIDECHAIN_RATIO,
      }

      /// Note control type.
      public enum NoteControlType {
        /// Gain in linear amplitude.
        [InspectorName("Gain")] GAIN = 0,
        /// Pitch shift.
        [InspectorName("Pitch Shift")] PITCH_SHIFT,
      }

      /// Returns a control of an engine.
      ///
      /// @param type Engine control type.
      /// @return Engine control value.
      public static float Engine_GetControl(EngineControlType type) {
        float value = 0.0f;
        if (!BarelyEngine_GetControl(Handle, type, ref value) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get engine engine control");
        }
        return value;
      }

      /// Returns the tempo of an engine.
      ///
      /// @return Tempo in beats per minute.
      public static double Engine_GetTempo() {
        double tempo = 0.0;
        if (!BarelyEngine_GetTempo(Handle, ref tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get engine tempo");
        }
        return tempo;
      }

      /// Returns the timestamp of an engine.
      ///
      /// @return Timestamp in seconds.
      public static double Engine_GetTimestamp() {
        double timestamp = 0.0;
        if (!BarelyEngine_GetTimestamp(Handle, ref timestamp) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get engine timestamp");
        }
        return timestamp;
      }

      /// Sets a control of an engine.
      ///
      /// @param type Engine control type.
      /// @param value Engine control value.
      public static void Engine_SetControl(EngineControlType type, float value) {
        if (!BarelyEngine_SetControl(Handle, type, value) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set engine engine control");
        }
      }

      /// Sets the tempo of an engine.
      ///
      /// @param tempo Tempo in beats per minute.
      public static void Engine_SetTempo(double tempo) {
        if (!BarelyEngine_SetTempo(Handle, tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set engine tempo");
        }
      }

      /// Creates a new instrument.
      ///
      /// @param instrument Instrument.
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_Create(Instrument instrument, ref IntPtr instrumentHandle) {
#if UNITY_EDITOR
        if (!Application.isPlaying) {
          _isShuttingDown = false;
        }
#endif  // UNITY_EDITOR
        if (Handle == IntPtr.Zero || instrumentHandle != IntPtr.Zero) {
          return;
        }
        _instrumentControlOverrides[(int)InstrumentControlType.GAIN].value = instrument.Gain;
        _instrumentControlOverrides[(int)InstrumentControlType.PITCH_SHIFT].value =
            instrument.PitchShift;
        _instrumentControlOverrides[(int)InstrumentControlType.RETRIGGER].value =
            instrument.Retrigger ? 1.0f : 0.0f;
        _instrumentControlOverrides[(int)InstrumentControlType.STEREO_PAN].value =
            (float)instrument.StereoPan;
        _instrumentControlOverrides[(int)InstrumentControlType.VOICE_COUNT].value =
            (float)instrument.VoiceCount;
        _instrumentControlOverrides[(int)InstrumentControlType.ATTACK].value = instrument.Attack;
        _instrumentControlOverrides[(int)InstrumentControlType.DECAY].value = instrument.Decay;
        _instrumentControlOverrides[(int)InstrumentControlType.SUSTAIN].value = instrument.Sustain;
        _instrumentControlOverrides[(int)InstrumentControlType.RELEASE].value = instrument.Release;
        _instrumentControlOverrides[(int)InstrumentControlType.OSC_MIX].value = instrument.OscMix;
        _instrumentControlOverrides[(int)InstrumentControlType.OSC_MODE].value =
            (float)instrument.OscMode;
        _instrumentControlOverrides[(int)InstrumentControlType.OSC_NOISE_MIX].value =
            instrument.OscNoiseMix;
        _instrumentControlOverrides[(int)InstrumentControlType.OSC_PITCH_SHIFT].value =
            instrument.OscPitchShift;
        _instrumentControlOverrides[(int)InstrumentControlType.OSC_SHAPE].value =
            instrument.OscShape;
        _instrumentControlOverrides[(int)InstrumentControlType.OSC_SKEW].value = instrument.OscSkew;
        _instrumentControlOverrides[(int)InstrumentControlType.SLICE_MODE].value =
            (float)instrument.SliceMode;
        _instrumentControlOverrides[(int)InstrumentControlType.BIT_CRUSHER_DEPTH].value =
            instrument.BitCrusherDepth;
        _instrumentControlOverrides[(int)InstrumentControlType.BIT_CRUSHER_RATE].value =
            instrument.BitCrusherRate;
        _instrumentControlOverrides[(int)InstrumentControlType.DISTORTION_AMOUNT].value =
            instrument.DistortionAmount;
        _instrumentControlOverrides[(int)InstrumentControlType.DISTORTION_DRIVE].value =
            instrument.DistortionDrive;
        _instrumentControlOverrides[(int)InstrumentControlType.FILTER_TYPE].value =
            (float)instrument.FilterType;
        _instrumentControlOverrides[(int)InstrumentControlType.FILTER_FREQUENCY].value =
            instrument.FilterFrequency;
        _instrumentControlOverrides[(int)InstrumentControlType.FILTER_Q].value = instrument.FilterQ;
        _instrumentControlOverrides[(int)InstrumentControlType.DELAY_SEND].value =
            instrument.DelaySend;
        _instrumentControlOverrides[(int)InstrumentControlType.SIDECHAIN_SEND].value =
            instrument.SidechainSend;
        _instrumentControlOverrides[(int)InstrumentControlType.ARP_MODE].value =
            (float)instrument.ArpMode;
        _instrumentControlOverrides[(int)InstrumentControlType.ARP_GATE_RATIO].value =
            instrument.ArpGateRatio;
        _instrumentControlOverrides[(int)InstrumentControlType.ARP_RATE].value = instrument.ArpRate;
        bool success =
            BarelyInstrument_Create(Handle, _instrumentControlOverrides,
                                    _instrumentControlOverrides.Length, ref instrumentHandle);
        if (!success) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "'");
          return;
        }
        _instruments.Add(instrumentHandle, instrument);
        BarelyInstrument_SetNoteEventCallback(instrumentHandle, Instrument_OnNoteEvent,
                                              instrumentHandle);
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_Destroy(ref IntPtr instrumentHandle) {
        if (Handle == IntPtr.Zero || instrumentHandle == IntPtr.Zero) {
          instrumentHandle = IntPtr.Zero;
          return;
        }
        if (!BarelyInstrument_Destroy(instrumentHandle)) {
          Debug.LogError("Failed to destroy instrument");
        }
        _instruments.Remove(instrumentHandle);
        instrumentHandle = IntPtr.Zero;
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param type Instrument control type.
      /// @return Instrument control value.
      public static float Instrument_GetControl(IntPtr instrumentHandle,
                                                InstrumentControlType type) {
        float value = 0.0f;
        if (!BarelyInstrument_GetControl(instrumentHandle, type, ref value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument control " + type);
        }
        return value;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param type Note control type.
      /// @return Note control value.
      public static float Instrument_GetNoteControl(IntPtr instrumentHandle, float pitch,
                                                    NoteControlType type) {
        float value = 0.0f;
        if (!BarelyInstrument_GetNoteControl(instrumentHandle, pitch, type, ref value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument note " + pitch + " control " + type + " value");
        }
        return value;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(IntPtr instrumentHandle, float pitch) {
        bool isNoteOn = false;
        if (!BarelyInstrument_IsNoteOn(instrumentHandle, pitch, ref isNoteOn) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if instrument note " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_SetAllNotesOff(IntPtr instrumentHandle) {
        if (!BarelyInstrument_SetAllNotesOff(instrumentHandle) && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes");
        }
      }

      /// Sets an instrument control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param type Instrument control type.
      /// @param value Instrument control value.
      public static void Instrument_SetControl(IntPtr instrumentHandle, InstrumentControlType type,
                                               float value) {
        if (!BarelyInstrument_SetControl(instrumentHandle, type, value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument control " + type + " value to " + value);
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param type Note control type.
      /// @param value Note control value.
      public static void Instrument_SetNoteControl(IntPtr instrumentHandle, float pitch,
                                                   NoteControlType type, float value) {
        if (!BarelyInstrument_SetNoteControl(instrumentHandle, pitch, type, value) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument note " + pitch + " control " + type +
                         " value to " + value);
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOff(IntPtr instrumentHandle, float pitch) {
        if (!BarelyInstrument_SetNoteOff(instrumentHandle, pitch) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + "");
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param gain Note gain.
      /// @param pitchShift Note pitch shift.
      public static void Instrument_SetNoteOn(IntPtr instrumentHandle, float pitch, float gain,
                                              float pitchShift) {
        _noteControlOverrides[(int)NoteControlType.GAIN].value = gain;
        _noteControlOverrides[(int)NoteControlType.PITCH_SHIFT].value = pitchShift;
        if (!BarelyInstrument_SetNoteOn(instrumentHandle, pitch, _noteControlOverrides,
                                        _noteControlOverrides.Length) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + gain + " gain");
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param instrumentSlices List of instrument slices.
      public static void Instrument_SetSampleData(IntPtr instrumentHandle,
                                                  List<Instrument.Slice> instrumentSlices) {
        Slice[] slices = null;
        if (instrumentSlices.Count > 0) {
          slices = new Slice[instrumentSlices.Count];
          for (int i = 0; i < slices.Length; ++i) {
            instrumentSlices[i].Sample.LoadAudioData();
            slices[i] = new Slice() {
              rootPitch = instrumentSlices[i].RootPitch / 12.0f,
              sampleRate =
                  (instrumentSlices[i].Data != null) ? instrumentSlices[i].Sample.frequency : 0,
              samples = instrumentSlices[i].Data,
              sampleCount =
                  (instrumentSlices[i].Data != null) ? instrumentSlices[i].Sample.samples : 0,
            };
          }
        }
        if (!BarelyInstrument_SetSampleData(instrumentHandle, slices, instrumentSlices.Count) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument sample data");
        }
      }

      /// Creates a new performer.
      ///
      /// @param performer Performer.
      /// @param performerRef Performer reference.
      public static void Performer_Create(Performer performer, ref UInt32 performerRef) {
        if (Handle == IntPtr.Zero || performerRef != 0) {
          return;
        }
        if (!BarelyEngine_CreatePerformer(Handle, ref performerRef)) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
        }
        _performers.Add(performerRef, performer);
      }

      /// Destroys a performer.
      ///
      /// @param performerRef Performer reference.
      public static void Performer_Destroy(ref UInt32 performerRef) {
        if (Handle == IntPtr.Zero || performerRef == 0) {
          performerRef = 0;
          return;
        }
        if (!BarelyEngine_DestroyPerformer(Handle, performerRef)) {
          Debug.LogError("Failed to destroy performer");
        }
        _performers.Remove(performerRef);
        performerRef = 0;
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerRef Performer reference.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(UInt32 performerRef) {
        double loopBeginPosition = 0.0;
        if (!BarelyPerformer_GetLoopBeginPosition(Handle, performerRef, ref loopBeginPosition) &&
            performerRef != 0) {
          Debug.LogError("Failed to get performer loop begin position");
        }
        return loopBeginPosition;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerRef Performer reference.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(UInt32 performerRef) {
        double loopLength = 0.0;
        if (!BarelyPerformer_GetLoopLength(Handle, performerRef, ref loopLength) &&
            performerRef != 0) {
          Debug.LogError("Failed to get performer loop length");
        }
        return loopLength;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerRef Performer reference.
      /// @return Position in beats.
      public static double Performer_GetPosition(UInt32 performerRef) {
        double position = 0.0;
        if (!BarelyPerformer_GetPosition(Handle, performerRef, ref position) && performerRef != 0) {
          Debug.LogError("Failed to get performer position");
        }
        return position;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerRef Performer reference.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(UInt32 performerRef) {
        bool isLooping = false;
        if (!BarelyPerformer_IsLooping(Handle, performerRef, ref isLooping) && performerRef != 0) {
          Debug.LogError("Failed to get if performer is looping");
        }
        return isLooping;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerRef Performer reference.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(UInt32 performerRef) {
        bool isPlaying = false;
        if (!BarelyPerformer_IsPlaying(Handle, performerRef, ref isPlaying) && performerRef != 0) {
          Debug.LogError("Failed to get if performer is playing");
        }
        return isPlaying;
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerRef Performer reference.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(UInt32 performerRef,
                                                        double loopBeginPosition) {
        if (!BarelyPerformer_SetLoopBeginPosition(Handle, performerRef, loopBeginPosition) &&
            performerRef != 0) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerRef Performer reference.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(UInt32 performerRef, double loopLength) {
        if (!BarelyPerformer_SetLoopLength(Handle, performerRef, loopLength) && performerRef != 0) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerRef Performer reference.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(UInt32 performerRef, bool isLooping) {
        if (!BarelyPerformer_SetLooping(Handle, performerRef, isLooping) && performerRef != 0) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerRef Performer reference.
      /// @param position Position in beats.
      public static void Performer_SetPosition(UInt32 performerRef, double position) {
        if (!BarelyPerformer_SetPosition(Handle, performerRef, position) && performerRef != 0) {
          Debug.LogError("Failed to set performer position");
        }
      }

      /// Starts a performer.
      ///
      /// @param performerRef Performer reference.
      public static void Performer_Start(UInt32 performerRef) {
        if (!BarelyPerformer_Start(Handle, performerRef) && performerRef != 0) {
          Debug.LogError("Failed to start performer");
        }
      }

      /// Stops a performer.
      ///
      /// @param performerRef Performer reference.
      public static void Performer_Stop(UInt32 performerRef) {
        if (!BarelyPerformer_Stop(Handle, performerRef) && performerRef != 0) {
          Debug.LogError("Failed to stop performer");
        }
      }

      /// Gets a scale note pitch for a given degree.
      ///
      /// @param scale Pointer to scale.
      /// @param degree Scale degree.
      /// @return Output note pitch.
      public static float Scale_GetPitch(Barely.Scale scale, int degree) {
        float pitch = 0.0f;
        _scale.pitches = scale.Pitches;
        _scale.pitchCount = scale.PitchCount;
        _scale.rootPitch = scale.RootPitch;
        _scale.mode = scale.Mode;
        if (!BarelyScale_GetPitch(ref _scale, degree, ref pitch)) {
          Debug.LogError("Failed to get scale " + scale + " note pitch with a degree " + degree);
        }
        return pitch;
      }

      /// Creates a new task.
      ///
      /// @param task Task.
      /// @param performerRef Performer reference.
      /// @param position Task position in beats.
      /// @param duration Task duration in beats.
      /// @param priority Task priority.
      /// @param taskHandle Task handle.
      public static void Task_Create(Task task, UInt32 performerRef, double position,
                                     double duration, int priority, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle != IntPtr.Zero) {
          return;
        }
        if (!BarelyTask_Create(Handle, performerRef, position, Math.Max(duration, _minTaskDuration),
                               priority, Task_OnEvent, IntPtr.Zero, ref taskHandle)) {
          Debug.LogError("Failed to create task '" + task + "'");
          return;
        }
        _tasks.Add(taskHandle, task);
        BarelyTask_SetEventCallback(taskHandle, Task_OnEvent, taskHandle);
      }

      /// Destroys a task.
      ///
      /// @param performerRef Performer reference.
      /// @param taskHandle Task handle.
      public static void Task_Destroy(UInt32 performerRef, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || performerRef == 0 || taskHandle == IntPtr.Zero) {
          taskHandle = IntPtr.Zero;
          return;
        }
        if (!BarelyTask_Destroy(taskHandle)) {
          Debug.LogError("Failed to destroy performer task");
        }
        _tasks.Remove(taskHandle);
        taskHandle = IntPtr.Zero;
      }

      /// Returns the duration of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Duration in beats.
      public static double Task_GetDuration(IntPtr taskHandle) {
        double duration = 0.0;
        if (!BarelyTask_GetDuration(taskHandle, ref duration) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task duration");
        }
        return (duration > _minTaskDuration) ? duration : 0.0;
      }

      /// Returns the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Position in beats.
      public static double Task_GetPosition(IntPtr taskHandle) {
        double position = 0.0;
        if (!BarelyTask_GetPosition(taskHandle, ref position) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position");
        }
        return position;
      }

      /// Returns the priority of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Priority.
      public static int Task_GetPriority(IntPtr taskHandle) {
        int priority = 0;
        if (!BarelyTask_GetPriority(taskHandle, ref priority) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task priority");
        }
        return priority;
      }

      /// Returns whether a task is active or not.
      ///
      /// @param taskHandle Task handle.
      /// @return True if active, false otherwise.
      public static bool Task_IsActive(IntPtr taskHandle) {
        bool isActive = false;
        if (!BarelyTask_IsActive(taskHandle, ref isActive) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer task is active");
        }
        return isActive;
      }

      /// Sets the duration of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param duration Task duration in beats.
      public static void Task_SetDuration(IntPtr taskHandle, double duration) {
        if (!BarelyTask_SetDuration(taskHandle, Math.Max(duration, _minTaskDuration)) &&
            taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task duration");
        }
      }

      /// Sets the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param position Task position in beats.
      public static void Task_SetPosition(IntPtr taskHandle, double position) {
        if (!BarelyTask_SetPosition(taskHandle, position) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      /// Sets the priority of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param priority Task priority.
      public static void Task_SetPriority(IntPtr taskHandle, int priority) {
        if (!BarelyTask_SetPriority(taskHandle, priority) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task priority");
        }
      }

      // Note event callback.
      private delegate void NoteEventCallback(NoteEventType type, float pitch, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteEventCallback))]
      private static void Instrument_OnNoteEvent(NoteEventType type, float pitch, IntPtr userData) {
        if (_instruments.TryGetValue(userData, out var instrument)) {
          if (type == NoteEventType.BEGIN) {
            Instrument.Internal.OnNoteOn(instrument, pitch);
          } else if (type == NoteEventType.END) {
            Instrument.Internal.OnNoteOff(instrument, pitch);
          } else {
            Debug.LogError("Invalid note event type");
          }
        }
      }

      // Task event callback.
      private delegate void TaskEventCallback(TaskEventType type, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(TaskEventCallback))]
      private static void Task_OnEvent(TaskEventType type, IntPtr userData) {
        if (_tasks.TryGetValue(userData, out var task)) {
          Task.Internal.OnProcess(task, type);
        }
      }

      // Instrument control override.
      [StructLayout(LayoutKind.Sequential)]
      private struct InstrumentControlOverride {
        // Type.
        public InstrumentControlType type;

        // Value.
        public float value;
      }

      // Instrument control override.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteControlOverride {
        // Type.
        public NoteControlType type;

        // Value.
        public float value;
      }

      // Note event types.
      private enum NoteEventType {
        // Begin.
        [InspectorName("BEGIN")] BEGIN = 0,
        // End.
        [InspectorName("END")] END,
      }

      // Slice of sample data.
      [StructLayout(LayoutKind.Sequential)]
      private struct Slice {
        // Root note pitch.
        public float rootPitch;

        // Sampling rate in hertz.
        public Int32 sampleRate;

        // Array of mono samples.
        public float[] samples;

        // Number of mono samples.
        public Int32 sampleCount;
      }

      [StructLayout(LayoutKind.Sequential)]
      private struct Scale {
        // Array of note pitches relative to the root note pitch.
        public float[] pitches;

        // Number of note pitches.
        public Int32 pitchCount;

        // Root note pitch of the scale.
        public float rootPitch;

        // Mode index.
        public Int32 mode;
      }

      // Singleton engine handle.
      private static IntPtr Handle {
        get {
          if (_isShuttingDown) {
            return IntPtr.Zero;
          }
          if (_handle == IntPtr.Zero) {
            var state =
                new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<State>();
            if (Application.isPlaying) {
              GameObject.DontDestroyOnLoad(state.gameObject);
            }
            if (_handle == IntPtr.Zero) {
              GameObject.DestroyImmediate(state.gameObject);
              _isShuttingDown = true;
            }
          }
          return _handle;
        }
      }
      private static IntPtr _handle = IntPtr.Zero;

      // Minimum task duration to avoid zero duration tasks.
      private const double _minTaskDuration = 1e-6;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Map of instruments by their handles.
      private static Dictionary<IntPtr, Instrument> _instruments = null;

      // Array of instrument control overrides.
      private static InstrumentControlOverride[] _instrumentControlOverrides = null;

      // Array of note control overrides.
      private static NoteControlOverride[] _noteControlOverrides = null;

      // Map of performers by their references.
      private static Dictionary<UInt32, Performer> _performers = null;

      // Map of tasks by their handles.
      private static Dictionary<IntPtr, Task> _tasks = null;

      // Scale.
      private static Scale _scale = new Scale {
        pitches = null,
        pitchCount = 0,
        rootPitch = 0.0f,
        mode = 0,
      };

      // Component that manages internal state.
      [ExecuteInEditMode]
      [RequireComponent(typeof(AudioSource))]
      private class State : MonoBehaviour {
        private void Awake() {
#if UNITY_EDITOR
          UnityEditor.AssemblyReloadEvents.beforeAssemblyReload += OnAssemblyReload;
          UnityEditor.EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
#endif  // UNITY_EDITOR
          AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
          Initialize();
          var source = GetComponent<AudioSource>();
          source.loop = true;
          source.clip = AudioClip.Create("zeros", 64, 1, AudioSettings.outputSampleRate, false);
          source.Play();
        }

        private void OnDestroy() {
#if UNITY_EDITOR
          UnityEditor.AssemblyReloadEvents.beforeAssemblyReload -= OnAssemblyReload;
          UnityEditor.EditorApplication.playModeStateChanged -= OnPlayModeStateChanged;
#endif  // UNITY_EDITOR
          AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
          Shutdown();
        }

#if UNITY_EDITOR
        private void OnAssemblyReload() {
          if (Application.isPlaying) {
            GameObject.Destroy(gameObject);
          } else {
            _isShuttingDown = true;
            GameObject.DestroyImmediate(gameObject);
          }
        }

        private void OnPlayModeStateChanged(UnityEditor.PlayModeStateChange state) {
          if (state == UnityEditor.PlayModeStateChange.ExitingEditMode) {
            _isShuttingDown = true;
            GameObject.DestroyImmediate(gameObject);
          }
        }
#endif  // UNITY_EDITOR

        private void OnApplicationQuit() {
          GameObject.Destroy(gameObject);
        }

        private void OnAudioConfigurationChanged(bool deviceWasChanged) {
          Shutdown();
          var instruments = new List<Instrument>(_instruments.Values);
          for (int i = 0; i < instruments.Count; ++i) {
            instruments[i].enabled = false;
          }
          var performers = new List<Performer>(_performers.Values);
          for (int i = 0; i < performers.Count; ++i) {
            performers[i].enabled = false;
          }
          Initialize();
          for (int i = 0; i < instruments.Count; ++i) {
            instruments[i].enabled = true;
          }
          for (int i = 0; i < performers.Count; ++i) {
            performers[i].enabled = true;
          }
        }

        private void OnAudioFilterRead(float[] data, int channels) {
          BarelyEngine_Process(_handle, data, channels, data.Length / channels,
                               AudioSettings.dspTime);
        }

        private void LateUpdate() {
          BarelyEngine_Update(_handle, GetNextTimestamp());
        }

        // Initializes the internal state.
        private void Initialize() {
          _isShuttingDown = false;
          var config = AudioSettings.GetConfiguration();
          if (!BarelyEngine_Create(config.sampleRate, ref _handle)) {
            Debug.LogError("Failed to initialize BarelyEngine");
            return;
          }
          BarelyEngine_SetTempo(_handle, _tempo);
          BarelyEngine_SetControl(_handle, EngineControlType.COMPRESSOR_MIX, _compressorMix);
          BarelyEngine_SetControl(_handle, EngineControlType.COMPRESSOR_THRESHOLD,
                                  _compressorThreshold);
          BarelyEngine_SetControl(_handle, EngineControlType.COMPRESSOR_ATTACK, _compressorAttack);
          BarelyEngine_SetControl(_handle, EngineControlType.COMPRESSOR_RELEASE,
                                  _compressorRelease);
          BarelyEngine_SetControl(_handle, EngineControlType.COMPRESSOR_RATIO, _compressorRatio);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_MIX, _delayMix);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_TIME, _delayTime);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_FEEDBACK, _delayFeedback);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_LOW_PASS_FREQUENCY,
                                  _delayLowPassFrequency);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_HIGH_PASS_FREQUENCY,
                                  _delayHighPassFrequency);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_MIX, _sidechainMix);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_THRESHOLD,
                                  _sidechainThreshold);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_ATTACK, _sidechainAttack);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_RELEASE, _sidechainRelease);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_RATIO, _sidechainRatio);
          _dspLatency = (float)(config.dspBufferSize + 1) / config.sampleRate;
          _instruments = new Dictionary<IntPtr, Instrument>();
          _instrumentControlOverrides =
              new InstrumentControlOverride[Enum.GetNames(typeof(InstrumentControlType)).Length];
          for (int i = 0; i < _instrumentControlOverrides.Length; ++i) {
            _instrumentControlOverrides[i].type = (InstrumentControlType)i;
          }
          _noteControlOverrides =
              new NoteControlOverride[Enum.GetNames(typeof(NoteControlType)).Length];
          for (int i = 0; i < _noteControlOverrides.Length; ++i) {
            _noteControlOverrides[i].type = (NoteControlType)i;
          }
          _performers = new Dictionary<UInt32, Performer>();
          _tasks = new Dictionary<IntPtr, Task>();
          BarelyEngine_Update(_handle, GetNextTimestamp());
        }

        // Shuts down the internal state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyEngine_Destroy(_handle);
          _handle = IntPtr.Zero;
        }

        // Returns the next timestamp to update.
        private double GetNextTimestamp() {
          return Math.Max(AudioSettings.dspTime + Math.Max(_dspLatency, _minLookahead),
                          Timestamp + (double)Time.deltaTime);
        }

        // Minimum lookahead time, set to an empirical value that can be adjusted as needed.
        private const double _minLookahead = 0.025;

        // DSP latency in seconds.
        private double _dspLatency = 0.0;
      }

#if !UNITY_EDITOR && UNITY_IOS
      private const string _pluginName = "__Internal";
#else
      private const string _pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Create")]
      private static extern bool BarelyEngine_Create(Int32 sampleRate, ref IntPtr outEngine);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Destroy")]
      private static extern bool BarelyEngine_Destroy(IntPtr engine);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetControl")]
      private static extern bool BarelyEngine_GetControl(IntPtr engine, EngineControlType type,
                                                         ref float value);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetTempo")]
      private static extern bool BarelyEngine_GetTempo(IntPtr engine, ref double outTempo);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetTimestamp")]
      private static extern bool BarelyEngine_GetTimestamp(IntPtr engine, ref double outTimestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Process")]
      private static extern bool BarelyEngine_Process(IntPtr engine,
                                                      [In, Out] float[] outputSamples,
                                                      Int32 outputChannelCount,
                                                      Int32 outputFrameCount, double timestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_SetControl")]
      private static extern bool BarelyEngine_SetControl(IntPtr engine, EngineControlType type,
                                                         float value);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_SetTempo")]
      private static extern bool BarelyEngine_SetTempo(IntPtr engine, double tempo);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Update")]
      private static extern bool BarelyEngine_Update(IntPtr engine, double timestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_Create")]
      private static extern bool BarelyInstrument_Create(
          IntPtr engine, [In] InstrumentControlOverride[] controlOverrides,
          Int32 controlOverrideCount, ref IntPtr outInstrument);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern bool BarelyInstrument_Destroy(IntPtr instrument);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern bool BarelyInstrument_GetControl(IntPtr instrument,
                                                             InstrumentControlType type,
                                                             ref float outValue);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern bool BarelyInstrument_GetNoteControl(IntPtr instrument, float pitch,
                                                                 NoteControlType type,
                                                                 ref float outValue);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern bool BarelyInstrument_IsNoteOn(IntPtr instrument, float pitch,
                                                           ref bool outIsNoteOn);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetAllNotesOff")]
      private static extern bool BarelyInstrument_SetAllNotesOff(IntPtr instrument);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern bool BarelyInstrument_SetControl(IntPtr instrument,
                                                             InstrumentControlType type,
                                                             float value);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern bool BarelyInstrument_SetNoteControl(IntPtr instrument, float pitch,
                                                                 NoteControlType type, float value);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteEventCallback")]
      private static extern bool BarelyInstrument_SetNoteEventCallback(IntPtr instrument,
                                                                       NoteEventCallback callback,
                                                                       IntPtr userData);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern bool BarelyInstrument_SetNoteOff(IntPtr instrument, float pitch);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern bool BarelyInstrument_SetNoteOn(
          IntPtr instrument, float pitch, [In] NoteControlOverride[] noteControlOverrides,
          Int32 noteControlOverrideCount);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetSampleData")]
      private static extern bool BarelyInstrument_SetSampleData(IntPtr instrument,
                                                                [In] Slice[] slices,
                                                                Int32 sliceCount);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_CreatePerformer")]
      private static extern bool BarelyEngine_CreatePerformer(IntPtr engine,
                                                              ref UInt32 outPerformer);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_DestroyPerformer")]
      private static extern bool BarelyEngine_DestroyPerformer(IntPtr engine, UInt32 performerRef);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetLoopBeginPosition")]
      private static extern bool BarelyPerformer_GetLoopBeginPosition(
          IntPtr engine, UInt32 performerRef, ref double outLoopBeginPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetLoopLength")]
      private static extern bool BarelyPerformer_GetLoopLength(IntPtr engine, UInt32 performerRef,
                                                               ref double outLoopLength);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern bool BarelyPerformer_GetPosition(IntPtr engine, UInt32 performerRef,
                                                             ref double outPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_IsLooping")]
      private static extern bool BarelyPerformer_IsLooping(IntPtr engine, UInt32 performerRef,
                                                           ref bool outIsLooping);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_IsPlaying")]
      private static extern bool BarelyPerformer_IsPlaying(IntPtr engine, UInt32 performerRef,
                                                           ref bool outIsPlaying);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLoopBeginPosition")]
      private static extern bool BarelyPerformer_SetLoopBeginPosition(IntPtr engine,
                                                                      UInt32 performerRef,
                                                                      double loopBeginPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLoopLength")]
      private static extern bool BarelyPerformer_SetLoopLength(IntPtr engine, UInt32 performerRef,
                                                               double loopLength);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLooping")]
      private static extern bool BarelyPerformer_SetLooping(IntPtr engine, UInt32 performerRef,
                                                            bool isLooping);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetPosition")]
      private static extern bool BarelyPerformer_SetPosition(IntPtr engine, UInt32 performerRef,
                                                             double position);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Start")]
      private static extern bool BarelyPerformer_Start(IntPtr engine, UInt32 performerRef);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Stop")]
      private static extern bool BarelyPerformer_Stop(IntPtr engine, UInt32 performerRef);

      [DllImport(_pluginName, EntryPoint = "BarelyScale_GetPitch")]
      private static extern bool BarelyScale_GetPitch([In] ref Scale scale, Int32 degree,
                                                      ref float outPitch);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_Create")]
      private static extern bool BarelyTask_Create(IntPtr engine, UInt32 performerRef,
                                                   double position, double duration, Int32 priority,
                                                   TaskEventCallback callback, IntPtr userData,
                                                   ref IntPtr outTask);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_Destroy")]
      private static extern bool BarelyTask_Destroy(IntPtr task);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_GetDuration")]
      private static extern bool BarelyTask_GetDuration(IntPtr task, ref double outDuration);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_GetPosition")]
      private static extern bool BarelyTask_GetPosition(IntPtr task, ref double outPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_GetPriority")]
      private static extern bool BarelyTask_GetPriority(IntPtr task, ref Int32 outPriority);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_IsActive")]
      private static extern bool BarelyTask_IsActive(IntPtr task, ref bool outIsActive);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetDuration")]
      private static extern bool BarelyTask_SetDuration(IntPtr task, double duration);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetPosition")]
      private static extern bool BarelyTask_SetPosition(IntPtr task, double position);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetPriority")]
      private static extern bool BarelyTask_SetPriority(IntPtr task, Int32 priority);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetEventCallback")]
      private static extern bool BarelyTask_SetEventCallback(IntPtr task,
                                                             TaskEventCallback callback,
                                                             IntPtr userData);
    }
  }
}  // namespace Barely
