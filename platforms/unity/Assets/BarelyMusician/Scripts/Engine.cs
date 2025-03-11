using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of an engine that governs all musical components.
  public static class Engine {
    /// Reference frequency in hertz (C4 by default).
    public static float ReferenceFrequency {
      get { return _referenceFrequency; }
      set {
        if (_referenceFrequency != value) {
          Internal.Engine_SetReferenceFrequency(value);
          _referenceFrequency = Internal.Engine_GetReferenceFrequency();
        }
      }
    }
    private static float _referenceFrequency = 440.0f * Mathf.Pow(2.0f, -9.0f / 12.0f);

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

    /// Schedules a task at a specific time.
    ///
    /// @param callback Task process callback.
    /// @param dspTime Time in seconds.
    public static void ScheduleTask(Action callback, double dspTime) {
      Internal.Engine_ScheduleTask(callback, dspTime);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Control type.
      public enum ControlType {
        /// Gain in decibels.
        [InspectorName("Gain")] GAIN = 0,
        /// Pitch shift.
        [InspectorName("Pitch Shift")] PITCH_SHIFT,
        /// Retrigger.
        [InspectorName("Retrigger")] RETRIGGER,
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
        /// Filter type.
        [InspectorName("Filter Type")] FILTER_TYPE,
        /// Filter frequency in hertz.
        [InspectorName("Filter Frequency")] FILTER_FREQUENCY,
        /// Filter Q factor.
        [InspectorName("Filter Q")] FILTER_Q,
        /// Bit crusher depth.
        [InspectorName("Bit Crusher Depth")] BIT_CRUSHER_DEPTH,
        /// Bit crusher rate.
        [InspectorName("Bit Crusher Rate")] BIT_CRUSHER_RATE,
      }

      /// Returns whether an arpeggiator note is on or not.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Arpeggiator_IsNoteOn(IntPtr arpeggiatorHandle, float pitch) {
        bool isNoteOn = false;
        if (!BarelyArpeggiator_IsNoteOn(arpeggiatorHandle, pitch, ref isNoteOn) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator note " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Returns whether an arpeggiator is playing or not.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @return True if playing, false otherwise.
      public static bool Arpeggiator_IsPlaying(IntPtr arpeggiatorHandle) {
        bool isPlaying = false;
        if (!BarelyArpeggiator_IsPlaying(arpeggiatorHandle, ref isPlaying) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator is playing");
        }
        return isPlaying;
      }

      /// Sets all arpeggiator notes off.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      public static void Arpeggiator_SetAllNotesOff(IntPtr arpeggiatorHandle) {
        if (!BarelyArpeggiator_SetAllNotesOff(arpeggiatorHandle) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all arpeggiator notes");
        }
      }

      /// Sets an arpeggiator gate ratio.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param gateRatio Gate ratio.
      public static void Arpeggiator_SetGateRatio(IntPtr arpeggiatorHandle, float gateRatio) {
        if (!BarelyArpeggiator_SetGateRatio(arpeggiatorHandle, gateRatio) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator gate ratio " + gateRatio);
        }
      }

      /// Sets an arpeggiator instrument.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param instrument Instrument.
      public static void Arpeggiator_SetInstrument(IntPtr arpeggiatorHandle,
                                                   Instrument instrument) {
        if (!BarelyArpeggiator_SetInstrument(arpeggiatorHandle,
                                             Instrument.Internal.GetHandle(instrument)) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator instrument '" + instrument.name + "'");
        }
      }

      /// Sets an arpeggiator note off.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOff(IntPtr arpeggiatorHandle, float pitch) {
        if (!BarelyArpeggiator_SetNoteOff(arpeggiatorHandle, pitch) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator note on.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOn(IntPtr arpeggiatorHandle, float pitch) {
        if (!BarelyArpeggiator_SetNoteOn(arpeggiatorHandle, pitch) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator rate.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param rate Rate in notes per beat.
      public static void Arpeggiator_SetRate(IntPtr arpeggiatorHandle, double rate) {
        if (!BarelyArpeggiator_SetRate(arpeggiatorHandle, rate) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator rate " + rate);
        }
      }

      /// Sets an arpeggiator style.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param style Style.
      public static void Arpeggiator_SetStyle(IntPtr arpeggiatorHandle, ArpeggiatorStyle style) {
        if (!BarelyArpeggiator_SetStyle(arpeggiatorHandle, style) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator style " + style);
        }
      }

      /// Creates a new component.
      ///
      /// @param component Component.
      /// @param componentHandle Component handle.
      public static void Component_Create(Component component, ref IntPtr componentHandle) {
        if (Handle == IntPtr.Zero || componentHandle != IntPtr.Zero) {
          return;
        }
        switch (component) {
          case Arpeggiator arpeggiator:
            if (BarelyArpeggiator_Create(_handle, ref componentHandle)) {
              return;
            }
            break;
          case Repeater repeater:
            if (BarelyRepeater_Create(_handle, ref componentHandle)) {
              return;
            }
            break;
          default:
            Debug.LogError("Unsupported component type: " + component.GetType());
            return;
        }
        Debug.LogError("Failed to create component '" + component.name + "'");
      }

      /// Destroys a component.
      ///
      /// @param component Component.
      /// @param componentHandle Component handle.
      public static void Component_Destroy(Component component, ref IntPtr componentHandle) {
        if (Handle == IntPtr.Zero || componentHandle == IntPtr.Zero) {
          return;
        }
        bool success = true;
        switch (component) {
          case Arpeggiator arpeggiator:
            success = BarelyArpeggiator_Destroy(componentHandle);
            break;
          case Repeater repeater:
            success = BarelyRepeater_Destroy(componentHandle);
            break;
          default:
            Debug.LogError("Unsupported component type: " + component.GetType());
            return;
        }
        if (!success) {
          Debug.LogError("Failed to destroy component '" + component.name + "'");
        }
        componentHandle = IntPtr.Zero;
      }

      /// Returns the reference frequency of an engine.
      ///
      /// @return Reference frequency in hertz.
      public static float Engine_GetReferenceFrequency() {
        float referenceFrequency = 0.0f;
        if (!BarelyEngine_GetReferenceFrequency(Handle, ref referenceFrequency) &&
            _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get engine reference frequency");
        }
        return referenceFrequency;
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

      /// Schedules a new engine task.
      ///
      /// @param callback Task process callback.
      /// @param timestamp Task timestamp in seconds.
      public static void Engine_ScheduleTask(Action callback, double timestamp) {
        if (timestamp < Timestamp) {
          Debug.LogError("Failed to create engine task at " + timestamp);
          return;
        }
        List<Action> callbacks = null;
        if (_scheduledTaskCallbacks != null &&
            !_scheduledTaskCallbacks.TryGetValue(timestamp, out callbacks)) {
          callbacks = new List<Action>();
          _scheduledTaskCallbacks.Add(timestamp, callbacks);
        }
        callbacks?.Add(callback);
      }

      /// Sets the reference frequency of an engine.
      ///
      /// @param referenceFrequency Reference frequency in hertz.
      public static void Engine_SetReferenceFrequency(float referenceFrequency) {
        if (!BarelyEngine_SetReferenceFrequency(Handle, referenceFrequency) &&
            _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set engine reference frequency");
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
        if (Handle == IntPtr.Zero || instrumentHandle != IntPtr.Zero) {
          return;
        }
        bool success = BarelyInstrument_Create(Handle, ref instrumentHandle);
        if (!success) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "'");
          return;
        }
        _instruments.Add(instrumentHandle, instrument);
        BarelyInstrument_SetNoteOffCallback(instrumentHandle, Instrument_OnNoteOff,
                                            instrumentHandle);
        BarelyInstrument_SetNoteOnCallback(instrumentHandle, Instrument_OnNoteOn, instrumentHandle);
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
      /// @param type Control type.
      /// @return Control value.
      public static float Instrument_GetControl(IntPtr instrumentHandle, ControlType type) {
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

      /// Processes instrument output samples.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Instrument_Process(IntPtr instrumentHandle, float[] outputSamples,
                                            int outputChannelCount) {
        if (Handle == IntPtr.Zero) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
          return;
        }
        int outputFrameCount = outputSamples.Length / outputChannelCount;
        if (BarelyInstrument_Process(instrumentHandle, _outputSamples, outputFrameCount,
                                     AudioSettings.dspTime)) {
          for (int frame = 0; frame < outputFrameCount; ++frame) {
            for (int channel = 0; channel < outputChannelCount; ++channel) {
              outputSamples[frame * outputChannelCount + channel] *= _outputSamples[frame];
            }
          }
        } else {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
        }
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
      /// @param type Control type.
      /// @param value Control value.
      public static void Instrument_SetControl(IntPtr instrumentHandle, ControlType type,
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
      public static void Instrument_SetNoteOn(IntPtr instrumentHandle, float pitch,
                                              float intensity) {
        if (!BarelyInstrument_SetNoteOn(instrumentHandle, pitch, intensity) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity");
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
            slices[i] = new Slice() {
              rootPitch = instrumentSlices[i].RootPitch / 12.0f,
              sampleRate =
                  (instrumentSlices[i].Sample != null) ? instrumentSlices[i].Sample.frequency : 0,
              samples = instrumentSlices[i].Data,
              sampleCount =
                  (instrumentSlices[i].Sample != null) ? instrumentSlices[i].Sample.samples : 0,
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
      /// @param performerHandle Performer handle.
      public static void Performer_Create(Performer performer, ref IntPtr performerHandle) {
        if (Handle == IntPtr.Zero || performerHandle != IntPtr.Zero) {
          return;
        }
        if (!BarelyPerformer_Create(Handle, ref performerHandle)) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
        }
        _performers.Add(performerHandle, performer);
        BarelyPerformer_SetBeatCallback(performerHandle, Performer_OnBeat, performerHandle);
      }

      /// Destroys a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Destroy(ref IntPtr performerHandle) {
        if (Handle == IntPtr.Zero || performerHandle == IntPtr.Zero) {
          performerHandle = IntPtr.Zero;
          return;
        }
        if (!BarelyPerformer_Destroy(performerHandle)) {
          Debug.LogError("Failed to destroy performer");
        }
        _performers.Remove(performerHandle);
        performerHandle = IntPtr.Zero;
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(IntPtr performerHandle) {
        double loopBeginPosition = 0.0;
        if (!BarelyPerformer_GetLoopBeginPosition(performerHandle, ref loopBeginPosition) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop begin position");
        }
        return loopBeginPosition;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(IntPtr performerHandle) {
        double loopLength = 0.0;
        if (!BarelyPerformer_GetLoopLength(performerHandle, ref loopLength) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop length");
        }
        return loopLength;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Position in beats.
      public static double Performer_GetPosition(IntPtr performerHandle) {
        double position = 0.0;
        if (!BarelyPerformer_GetPosition(performerHandle, ref position) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer position");
        }
        return position;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerHandle Performer handle.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(IntPtr performerHandle) {
        bool isLooping = false;
        if (!BarelyPerformer_IsLooping(performerHandle, ref isLooping) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is looping");
        }
        return isLooping;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerHandle Performer handle.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(IntPtr performerHandle) {
        bool isPlaying = false;
        if (!BarelyPerformer_IsPlaying(performerHandle, ref isPlaying) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is playing");
        }
        return isPlaying;
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(IntPtr performerHandle,
                                                        double loopBeginPosition) {
        if (!BarelyPerformer_SetLoopBeginPosition(performerHandle, loopBeginPosition) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(IntPtr performerHandle, double loopLength) {
        if (!BarelyPerformer_SetLoopLength(performerHandle, loopLength) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerHandle Performer handle.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(IntPtr performerHandle, bool isLooping) {
        if (!BarelyPerformer_SetLooping(performerHandle, isLooping) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param position Position in beats.
      public static void Performer_SetPosition(IntPtr performerHandle, double position) {
        if (!BarelyPerformer_SetPosition(performerHandle, position) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer position");
        }
      }

      /// Starts a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Start(IntPtr performerHandle) {
        if (!BarelyPerformer_Start(performerHandle) && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start performer");
        }
      }

      /// Stops a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Stop(IntPtr performerHandle) {
        if (!BarelyPerformer_Stop(performerHandle) && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop performer");
        }
      }

      /// Returns whether an repeater is playing or not.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @return True if playing, false otherwise.
      public static bool Repeater_IsPlaying(IntPtr repeaterHandle) {
        bool isPlaying = false;
        if (!BarelyRepeater_IsPlaying(repeaterHandle, ref isPlaying) &&
            repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if repeater is playing");
        }
        return isPlaying;
      }

      /// Pops the last note from the end.
      ///
      /// @param repeaterHandle Repeater handle.
      public static void Repeater_Pop(IntPtr repeaterHandle) {
        if (!BarelyRepeater_Pop(repeaterHandle) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Pushes a new note to the end.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param pitchOr Note pitch value or silence.
      /// @param length Note length in beats.
      public static void Repeater_Push(IntPtr repeaterHandle, float? pitchOr, int length) {
        if ((pitchOr.HasValue && !BarelyRepeater_Push(repeaterHandle, pitchOr.Value, length)) ||
            (!pitchOr.HasValue && !BarelyRepeater_PushSilence(repeaterHandle, length)) &&
                repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Sets an repeater instrument.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param instrument Instrument.
      public static void Repeater_SetInstrument(IntPtr repeaterHandle, Instrument instrument) {
        if (!BarelyRepeater_SetInstrument(repeaterHandle,
                                          Instrument.Internal.GetHandle(instrument)) &&
            repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater instrument '" + instrument.name + "'");
        }
      }

      /// Sets an repeater rate.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param rate Rate in notes per beat.
      public static void Repeater_SetRate(IntPtr repeaterHandle, double rate) {
        if (!BarelyRepeater_SetRate(repeaterHandle, rate) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater rate " + rate);
        }
      }

      /// Sets an repeater style.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param style Style.
      public static void Repeater_SetStyle(IntPtr repeaterHandle, RepeaterStyle style) {
        if (!BarelyRepeater_SetStyle(repeaterHandle, style) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater style " + style);
        }
      }

      /// Starts a repeater.
      ///
      /// @param repeaterHandle Repeater handle.
      /// @param pitchOffset Pitch offset.
      public static void Repeater_Start(IntPtr repeaterHandle, float pitchOffset) {
        if (!BarelyRepeater_Start(repeaterHandle, pitchOffset) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start repeater with a pitch offset" + pitchOffset);
        }
      }

      /// Stops a repeater.
      ///
      /// @param repeaterHandle Repeater handle.
      public static void Repeater_Stop(IntPtr repeaterHandle) {
        if (!BarelyRepeater_Stop(repeaterHandle) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop repeater");
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
      /// @param performerHandle Performer handle.
      /// @param position Task position.
      /// @param duration Task duration.
      /// @param callback Task process callback.
      /// @param taskHandle Task handle.
      public static void Task_Create(Task task, IntPtr performerHandle, double position,
                                     double duration, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle != IntPtr.Zero) {
          return;
        }
        if (!BarelyTask_Create(performerHandle, position, Math.Max(duration, _minTaskDuration),
                               Task_OnProcess, IntPtr.Zero, ref taskHandle)) {
          Debug.LogError("Failed to create task '" + task + "'");
          return;
        }
        _tasks.Add(taskHandle, task);
        BarelyTask_SetProcessCallback(taskHandle, Task_OnProcess, taskHandle);
      }

      /// Destroys a task.
      ///
      /// @param taskHandle Task handle.
      public static void Task_Destroy(IntPtr performerHandle, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || performerHandle == IntPtr.Zero || taskHandle == IntPtr.Zero) {
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
      /// @param duration Duration in beats.
      public static void Task_SetDuration(IntPtr taskHandle, double duration) {
        if (!BarelyTask_SetDuration(taskHandle, Math.Max(duration, _minTaskDuration)) &&
            taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task duration");
        }
      }

      /// Sets the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param position Position in beats.
      public static void Task_SetPosition(IntPtr taskHandle, double position) {
        if (!BarelyTask_SetPosition(taskHandle, position) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      // Instrument note off callback.
      private delegate void Instrument_NoteOffCallback(float pitch, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(Instrument_NoteOffCallback))]
      private static void Instrument_OnNoteOff(float pitch, IntPtr userData) {
        if (_instruments.TryGetValue(userData, out var instrument)) {
          Instrument.Internal.OnNoteOff(instrument, pitch);
        }
      }

      // Instrument note on callback.
      private delegate void Instrument_NoteOnCallback(float pitch, float intensity,
                                                      IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(Instrument_NoteOnCallback))]
      private static void Instrument_OnNoteOn(float pitch, float intensity, IntPtr userData) {
        if (_instruments.TryGetValue(userData, out var instrument)) {
          Instrument.Internal.OnNoteOn(instrument, pitch, intensity);
        }
      }

      // Performer beat callback.
      private delegate void Performer_BeatCallback(IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(Performer_BeatCallback))]
      private static void Performer_OnBeat(IntPtr userData) {
        if (_performers.TryGetValue(userData, out var performer)) {
          Performer.Internal.OnBeat(performer);
        }
      }

      // Task process callback.
      private delegate void Task_ProcessCallback(TaskState state, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(Task_ProcessCallback))]
      private static void Task_OnProcess(TaskState state, IntPtr userData) {
        if (_tasks.TryGetValue(userData, out var task)) {
          Task.Internal.OnProcess(task, state);
        }
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
            GameObject.DontDestroyOnLoad(state.gameObject);
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
      // TODO(#148): This can be removed once trigger tasks are supported.
      private const double _minTaskDuration = 1e-6;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Map of instruments by their handles.
      private static Dictionary<IntPtr, Instrument> _instruments = null;

      // Array of mono output samples.
      public static float[] _outputSamples = null;

      // Map of performers by their handles.
      private static Dictionary<IntPtr, Performer> _performers = null;

      // Map of scheduled list of task callbacks by their timestamps.
      private static SortedDictionary<double, List<Action>> _scheduledTaskCallbacks = null;

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
      private class State : MonoBehaviour {
        private void Awake() {
          AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
          Initialize();
        }

        private void OnDestroy() {
          AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
          Shutdown();
        }

        private void OnApplicationQuit() {
          GameObject.Destroy(gameObject);
        }

        private void OnAudioConfigurationChanged(bool deviceWasChanged) {
          Shutdown();
          var performers = new List<Performer>(_performers.Values);
          for (int i = 0; i < performers.Count; ++i) {
            performers[i].enabled = false;
          }
          var instruments = new List<Instrument>(_instruments.Values);
          for (int i = 0; i < instruments.Count; ++i) {
            instruments[i].enabled = false;
          }
          Initialize();
          for (int i = 0; i < instruments.Count; ++i) {
            instruments[i].enabled = true;
          }
          for (int i = 0; i < performers.Count; ++i) {
            performers[i].enabled = true;
          }
        }

        private void LateUpdate() {
          double nextTimestamp = GetNextTimestamp();
          while (_scheduledTaskCallbacks.Count > 0) {
            double taskTimestamp = _scheduledTaskCallbacks.ElementAt(0).Key;
            if (taskTimestamp > nextTimestamp) {
              break;
            }
            BarelyEngine_Update(_handle, taskTimestamp);
            var callbacks = _scheduledTaskCallbacks.ElementAt(0).Value;
            for (int i = 0; i < callbacks.Count; ++i) {
              callbacks[i]?.Invoke();
            }
            _scheduledTaskCallbacks.Remove(taskTimestamp);
          }
          BarelyEngine_Update(_handle, nextTimestamp);
        }

        // Initializes the internal state.
        private void Initialize() {
          _isShuttingDown = false;
          var config = AudioSettings.GetConfiguration();
          if (!BarelyEngine_Create(config.sampleRate, ref _handle)) {
            Debug.LogError("Failed to initialize BarelyEngine");
            return;
          }
          BarelyEngine_SetReferenceFrequency(_handle, _referenceFrequency);
          BarelyEngine_SetTempo(_handle, _tempo);
          _outputSamples = new float[config.dspBufferSize];
          _dspLatency = (float)(config.dspBufferSize + 1) / config.sampleRate;
          _instruments = new Dictionary<IntPtr, Instrument>();
          _performers = new Dictionary<IntPtr, Performer>();
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          _tasks = new Dictionary<IntPtr, Task>();
          BarelyEngine_Update(_handle, GetNextTimestamp());
        }

        // Shuts down the internal state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyEngine_Destroy(_handle);
          _handle = IntPtr.Zero;
          _scheduledTaskCallbacks = null;
        }

        // Returns the next timestamp to update.
        private double GetNextTimestamp() {
          return AudioSettings.dspTime +
                 Math.Max(Math.Max(_dspLatency, (double)Time.deltaTime), _minLookahead);
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

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_Create")]
      private static extern bool BarelyArpeggiator_Create(IntPtr engine, ref IntPtr outArpeggiator);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_Destroy")]
      private static extern bool BarelyArpeggiator_Destroy(IntPtr arpeggiator);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_IsNoteOn")]
      private static extern bool BarelyArpeggiator_IsNoteOn(IntPtr arpeggiator, float pitch,
                                                            ref bool outIsNoteOn);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_IsPlaying")]
      private static extern bool BarelyArpeggiator_IsPlaying(IntPtr arpeggiator,
                                                             ref bool outIsPlaying);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetAllNotesOff")]
      private static extern bool BarelyArpeggiator_SetAllNotesOff(IntPtr arpeggiator);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetGateRatio")]
      private static extern bool BarelyArpeggiator_SetGateRatio(IntPtr arpeggiator,
                                                                float gateRatio);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetInstrument")]
      private static extern bool BarelyArpeggiator_SetInstrument(IntPtr arpeggiator,
                                                                 IntPtr instrument);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetNoteOff")]
      private static extern bool BarelyArpeggiator_SetNoteOff(IntPtr arpeggiator, float gateRatio);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetNoteOn")]
      private static extern bool BarelyArpeggiator_SetNoteOn(IntPtr arpeggiator, float gateRatio);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetRate")]
      private static extern bool BarelyArpeggiator_SetRate(IntPtr arpeggiator, double rate);

      [DllImport(_pluginName, EntryPoint = "BarelyArpeggiator_SetStyle")]
      private static extern bool BarelyArpeggiator_SetStyle(IntPtr arpeggiator,
                                                            ArpeggiatorStyle style);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Create")]
      private static extern bool BarelyEngine_Create(Int32 sampleRate, ref IntPtr outEngine);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Destroy")]
      private static extern bool BarelyEngine_Destroy(IntPtr engine);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetReferenceFrequency")]
      private static extern bool BarelyEngine_GetReferenceFrequency(
          IntPtr engine, ref float outReferenceFrequency);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetTempo")]
      private static extern bool BarelyEngine_GetTempo(IntPtr engine, ref double outTempo);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetTimestamp")]
      private static extern bool BarelyEngine_GetTimestamp(IntPtr engine, ref double outTimestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_SetReferenceFrequency")]
      private static extern bool BarelyEngine_SetReferenceFrequency(IntPtr engine,
                                                                    float referenceFrequency);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_SetTempo")]
      private static extern bool BarelyEngine_SetTempo(IntPtr engine, double tempo);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Update")]
      private static extern bool BarelyEngine_Update(IntPtr engine, double timestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_Create")]
      private static extern bool BarelyInstrument_Create(IntPtr engine, ref IntPtr outInstrument);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern bool BarelyInstrument_Destroy(IntPtr instrument);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern bool BarelyInstrument_GetControl(IntPtr instrument, ControlType type,
                                                             ref float outValue);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern bool BarelyInstrument_GetNoteControl(IntPtr instrument, float pitch,
                                                                 NoteControlType type,
                                                                 ref float outValue);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern bool BarelyInstrument_IsNoteOn(IntPtr instrument, float pitch,
                                                           ref bool outIsNoteOn);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern bool BarelyInstrument_Process(IntPtr instrument,
                                                          [In, Out] float[] outputSamples,
                                                          Int32 outputSampleCount,
                                                          double timestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetAllNotesOff")]
      private static extern bool BarelyInstrument_SetAllNotesOff(IntPtr instrument);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern bool BarelyInstrument_SetControl(IntPtr instrument, ControlType type,
                                                             float value);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern bool BarelyInstrument_SetNoteControl(IntPtr instrument, float pitch,
                                                                 NoteControlType type, float value);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern bool BarelyInstrument_SetNoteOff(IntPtr instrument, float pitch);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOffCallback")]
      private static extern bool BarelyInstrument_SetNoteOffCallback(
          IntPtr instrument, Instrument_NoteOffCallback callback, IntPtr userData);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern bool BarelyInstrument_SetNoteOn(IntPtr instrument, float pitch,
                                                            float intensity);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOnCallback")]
      private static extern bool BarelyInstrument_SetNoteOnCallback(
          IntPtr instrument, Instrument_NoteOnCallback callback, IntPtr userData);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetSampleData")]
      private static extern bool BarelyInstrument_SetSampleData(IntPtr instrument,
                                                                [In] Slice[] slices,
                                                                Int32 sliceCount);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Create")]
      private static extern bool BarelyPerformer_Create(IntPtr engine, ref IntPtr outPerformer);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Destroy")]
      private static extern bool BarelyPerformer_Destroy(IntPtr performer);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetLoopBeginPosition")]
      private static extern bool BarelyPerformer_GetLoopBeginPosition(
          IntPtr performer, ref double outLoopBeginPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetLoopLength")]
      private static extern bool BarelyPerformer_GetLoopLength(IntPtr performer,
                                                               ref double outLoopLength);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern bool BarelyPerformer_GetPosition(IntPtr performer,
                                                             ref double outPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_IsLooping")]
      private static extern bool BarelyPerformer_IsLooping(IntPtr performer, ref bool outIsLooping);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_IsPlaying")]
      private static extern bool BarelyPerformer_IsPlaying(IntPtr performer, ref bool outIsPlaying);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetBeatCallback")]
      private static extern bool BarelyPerformer_SetBeatCallback(IntPtr performer,
                                                                 Performer_BeatCallback callback,
                                                                 IntPtr userData);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLoopBeginPosition")]
      private static extern bool BarelyPerformer_SetLoopBeginPosition(IntPtr performer,
                                                                      double loopBeginPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLoopLength")]
      private static extern bool BarelyPerformer_SetLoopLength(IntPtr performer, double loopLength);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLooping")]
      private static extern bool BarelyPerformer_SetLooping(IntPtr performer, bool isLooping);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetPosition")]
      private static extern bool BarelyPerformer_SetPosition(IntPtr performer, double position);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Start")]
      private static extern bool BarelyPerformer_Start(IntPtr performer);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Stop")]
      private static extern bool BarelyPerformer_Stop(IntPtr performer);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Clear")]
      private static extern bool BarelyRepeater_Clear(IntPtr repeater);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Create")]
      private static extern bool BarelyRepeater_Create(IntPtr engine, ref IntPtr outRepeater);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Destroy")]
      private static extern bool BarelyRepeater_Destroy(IntPtr repeater);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_IsPlaying")]
      private static extern bool BarelyRepeater_IsPlaying(IntPtr repeater, ref bool outIsPlaying);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Pop")]
      private static extern bool BarelyRepeater_Pop(IntPtr repeater);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Push")]
      private static extern bool BarelyRepeater_Push(IntPtr repeater, float pitch, Int32 length);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_PushSilence")]
      private static extern bool BarelyRepeater_PushSilence(IntPtr repeater, Int32 length);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_SetGateRatio")]
      private static extern bool BarelyRepeater_SetGateRatio(IntPtr repeater, float gateRatio);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_SetInstrument")]
      private static extern bool BarelyRepeater_SetInstrument(IntPtr repeater, IntPtr instrument);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_SetRate")]
      private static extern bool BarelyRepeater_SetRate(IntPtr repeater, double rate);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_SetStyle")]
      private static extern bool BarelyRepeater_SetStyle(IntPtr repeater, RepeaterStyle style);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Start")]
      private static extern bool BarelyRepeater_Start(IntPtr repeater, float pitchOffset);

      [DllImport(_pluginName, EntryPoint = "BarelyRepeater_Stop")]
      private static extern bool BarelyRepeater_Stop(IntPtr repeater);

      [DllImport(_pluginName, EntryPoint = "BarelyScale_GetPitch")]
      private static extern bool BarelyScale_GetPitch([In] ref Scale scale, Int32 degree,
                                                      ref float outPitch);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_Create")]
      private static extern bool BarelyTask_Create(IntPtr performer, double position,
                                                   double duration, Task_ProcessCallback callback,
                                                   IntPtr userData, ref IntPtr outTask);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_Destroy")]
      private static extern bool BarelyTask_Destroy(IntPtr task);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_GetDuration")]
      private static extern bool BarelyTask_GetDuration(IntPtr task, ref double outDuration);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_GetPosition")]
      private static extern bool BarelyTask_GetPosition(IntPtr task, ref double outPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_IsActive")]
      private static extern bool BarelyTask_IsActive(IntPtr performer, ref bool outIsActive);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetDuration")]
      private static extern bool BarelyTask_SetDuration(IntPtr task, double duration);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetPosition")]
      private static extern bool BarelyTask_SetPosition(IntPtr task, double position);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetProcessCallback")]
      private static extern bool BarelyTask_SetProcessCallback(IntPtr task,
                                                               Task_ProcessCallback callback,
                                                               IntPtr userData);
    }
  }
}  // namespace Barely
