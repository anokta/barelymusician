﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of a musician that governs the tempo for all musical components.
  public static class Musician {
    /// Tempo in beats per minute.
    public static double Tempo {
      get { return _tempo; }
      set {
        if (_tempo != value) {
          Internal.Musician_SetTempo(value);
          _tempo = Internal.Musician_GetTempo();
        }
      }
    }
    private static double _tempo = 120.0;

    /// Timestamp in seconds.
    public static double Timestamp {
      get { return Internal.Musician_GetTimestamp(); }
    }

    /// Returns the corresponding number of beats for a given number of `seconds`.
    ///
    /// @param seconds Number of seconds.
    /// @return Number of beats.
    public static double GetBeatsFromSeconds(double seconds) {
      return Internal.Musician_GetBeatsFromSeconds(seconds);
    }

    /// Returns the corresponding midi key for a given `pitch`.
    ///
    /// @param pitch Note pitch.
    /// @return Midi key.
    public static int GetMidiKeyFromPitch(double pitch) {
      return 69 + (int)(pitch * 12.0);
    }

    /// Returns the corresponding pitch for a given `midiKey`.
    ///
    /// @param midiKey Midi key.
    /// @param Note pitch.
    public static double GetPitchFromMidiKey(int midiKey) {
      return (double)(midiKey - 69) / 12.0;
    }

    /// Returns the corresponding number of seconds for a given number of `beats`.
    ///
    /// @param beats Number of beats.
    /// @return Number of seconds.
    public static double GetSecondsFromBeats(double beats) {
      return Internal.Musician_GetSecondsFromBeats(beats);
    }

    /// Schedules a task at a specific time.
    ///
    /// @param callback Task process callback.
    /// @param dspTime Time in seconds.
    public static void ScheduleTask(Action callback, double dspTime) {
      Internal.Musician_ScheduleTask(callback, dspTime);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Effect definition create callback signature.
      ///
      /// @param state Pointer to effect state.
      /// @param frame_rate Frame rate in hertz.
      public delegate void EffectDefinition_CreateCallback(ref IntPtr state, Int32 frameRate);

      /// Effect definition destroy callback signature.
      public delegate void EffectDefinition_DestroyCallback(ref IntPtr state);

      // Effect definition process callback signature.
      ///
      /// @param state Pointer to effect state.
      /// @param outputSamples Pointer to an array of interleaved output samples.
      /// @param outputChannelCount Number of output channels.
      /// @param outputFrameCount Number of output frames.
      public delegate void EffectDefinition_ProcessCallback(ref IntPtr state, IntPtr outputSamples,
                                                            Int32 outputChannelCount,
                                                            Int32 outputFrameCount);

      /// Effect definition set note control callback signature.
      ///
      /// @param state Pointer to effect state.
      /// @param controlId Control identifier.
      /// @param value Control value.
      public delegate void EffectDefinition_SetControlCallback(ref IntPtr state, Int32 controlId,
                                                               double value);

      /// Effect definition set data callback signature.
      ///
      /// @param state Pointer to effect state.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public delegate void EffectDefinition_SetDataCallback(ref IntPtr state, IntPtr data,
                                                            Int32 size);

      /// Instrument definition create callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param frame_rate Frame rate in hertz.
      public delegate void InstrumentDefinition_CreateCallback(ref IntPtr state, Int32 frameRate);

      /// Instrument definition destroy callback signature.
      public delegate void InstrumentDefinition_DestroyCallback(ref IntPtr state);

      // Instrument definition process callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param outputSamples Pointer to an array of interleaved output samples.
      /// @param outputChannelCount Number of output channels.
      /// @param outputFrameCount Number of output frames.
      public delegate void InstrumentDefinition_ProcessCallback(ref IntPtr state,
                                                                IntPtr outputSamples,
                                                                Int32 outputChannelCount,
                                                                Int32 outputFrameCount);

      /// Instrument definition set note control callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param controlId Control identifier.
      /// @param value Control value.
      public delegate void InstrumentDefinition_SetControlCallback(ref IntPtr state,
                                                                   Int32 controlId, double value);

      /// Instrument definition set data callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public delegate void InstrumentDefinition_SetDataCallback(ref IntPtr state, IntPtr data,
                                                                Int32 size);

      /// Instrument definition set note control callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param pitch Note pitch.
      /// @param controlId Note control identifier.
      /// @param value Note control value.
      public delegate void InstrumentDefinition_SetNoteControlCallback(ref IntPtr state,
                                                                       double pitch,
                                                                       Int32 controlId,
                                                                       double value);

      /// Instrument definition set note off callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param pitch Note pitch.
      public delegate void InstrumentDefinition_SetNoteOffCallback(ref IntPtr state, double pitch);

      /// Instrument definition set note on callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param noteId Note identifier.
      /// @param pitch Note pitch.
      /// @param intensity Note intensity.
      public delegate void InstrumentDefinition_SetNoteOnCallback(ref IntPtr state, Int32 noteId,
                                                                  double pitch, double intensity);

      /// Effect definition.
      [StructLayout(LayoutKind.Sequential)]
      public struct EffectDefinition {
        /// Create callback.
        public EffectDefinition_CreateCallback createCallback;

        /// Destroy callback.
        public EffectDefinition_DestroyCallback destroyCallback;

        /// Process callback.
        public EffectDefinition_ProcessCallback processCallback;

        /// Set control callback.
        public EffectDefinition_SetControlCallback setControlCallback;

        /// Set data callback.
        public EffectDefinition_SetDataCallback setDataCallback;

        /// Pointer to an array of control definitions.
        public IntPtr controlDefinitions;

        /// Number of control definitions.
        public Int32 controlDefinitionCount;
      }

      /// Instrument definition.
      [StructLayout(LayoutKind.Sequential)]
      public struct InstrumentDefinition {
        /// Create callback.
        public InstrumentDefinition_CreateCallback createCallback;

        /// Destroy callback.
        public InstrumentDefinition_DestroyCallback destroyCallback;

        /// Process callback.
        public InstrumentDefinition_ProcessCallback processCallback;

        /// Set control callback.
        public InstrumentDefinition_SetControlCallback setControlCallback;

        /// Set data callback.
        public InstrumentDefinition_SetDataCallback setDataCallback;

        /// Set note control callback.
        public InstrumentDefinition_SetNoteControlCallback setNoteControlCallback;

        /// Set note off callback.
        public InstrumentDefinition_SetNoteOffCallback setNoteOffCallback;

        /// Set note on callback.
        public InstrumentDefinition_SetNoteOnCallback setNoteOnCallback;

        /// Pointer to an array of control definitions.
        public IntPtr controlDefinitions;

        /// Number of control definitions.
        public Int32 controlDefinitionCount;

        /// Pointer to an array of note control definitions.
        public IntPtr noteControlDefinitions;

        /// Number of note control definitions.
        public Int32 noteControlDefinitionCount;
      }

      /// Custom effect interface.
      public interface CustomEffectInterface {
        /// Returns the effect definition.
        ///
        /// @return Effect definition.
        public EffectDefinition GetDefinition();
      }

      /// Custom instrument interface.
      public interface CustomInstrumentInterface {
        /// Returns the instrument definition.
        ///
        /// @return Instrument definition.
        public InstrumentDefinition GetDefinition();
      }

      /// Creates a new component.
      ///
      /// @param component Component.
      /// @param componentPtr Pointer to component.
      public static void Component_Create(Component component, ref IntPtr componentPtr) {
        if (Ptr == IntPtr.Zero || componentPtr != IntPtr.Zero) {
          return;
        }
        switch (component) {
          case Arpeggiator arpeggiator:
            if (BarelyArpeggiator_Create(_ptr, arpeggiator.ProcessOrder, ref componentPtr)) {
              return;
            }
            break;
          case Repeater repeater:
            if (BarelyRepeater_Create(_ptr, repeater.ProcessOrder, ref componentPtr)) {
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
      /// @param componentPtr Pointer to component.
      public static void Component_Destroy(Component component, ref IntPtr componentPtr) {
        if (Ptr == IntPtr.Zero || componentPtr == IntPtr.Zero) {
          return;
        }
        bool success = true;
        switch (component) {
          case Arpeggiator arpeggiator:
            success = BarelyArpeggiator_Destroy(componentPtr);
            break;
          case Repeater repeater:
            success = BarelyRepeater_Destroy(componentPtr);
            break;
          default:
            Debug.LogError("Unsupported component type: " + component.GetType());
            return;
        }
        if (!success) {
          Debug.LogError("Failed to destroy component '" + component.name + "'");
        }
        componentPtr = IntPtr.Zero;
      }

      /// Returns a control value.
      ///
      /// @param controlPtr Pointer to control.
      /// @return Control value.
      public static double Control_GetValue(IntPtr controlPtr) {
        double value = 0.0;
        if (!BarelyControl_GetValue(controlPtr, ref value) && controlPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get control value");
        }
        return value;
      }

      /// Resets a control value.
      ///
      /// @param controlPtr Pointer to control.
      public static void Control_ResetValue(IntPtr controlPtr) {
        if (!BarelyControl_ResetValue(controlPtr) && controlPtr != IntPtr.Zero) {
          Debug.LogError("Failed to reset control value");
        }
      }

      /// Sets a control value.
      ///
      /// @param controlPtr Pointer to control.
      /// @param value Control value.
      public static void Control_SetValue(IntPtr controlPtr, double value) {
        if (!BarelyControl_SetValue(controlPtr, value) && controlPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set control value to " + value);
        }
      }

      /// Creates a new effect.
      ///
      /// @param effect Effect.
      /// @param effectPtr Pointer to effect.
      public static void Effect_Create(Effect effect, ref IntPtr effectPtr) {
        if (Ptr == IntPtr.Zero || effectPtr != IntPtr.Zero) {
          return;
        }
        EffectDefinition definition;
        switch (effect) {
          case HighPassEffect highPass:
            definition = BarelyHighPassEffect_GetDefinition();
            break;
          case LfoEffect lfoEffect:
            definition = BarelyLfoEffect_GetDefinition();
            break;
          case LowPassEffect lowPass:
            definition = BarelyLowPassEffect_GetDefinition();
            break;
          case CustomEffectInterface custom:
            definition = custom.GetDefinition();
            break;
          default:
            Debug.LogError("Unsupported effect type: " + effect.GetType());
            return;
        }
        bool success = BarelyEffect_Create(Ptr, definition, ref effectPtr);
        if (effect.GetType().IsSubclassOf(typeof(CustomEffectInterface))) {
          if (definition.controlDefinitionCount > 0) {
            Marshal.FreeHGlobal(definition.controlDefinitions);
          }
        }
        if (!success) {
          Debug.LogError("Failed to create effect '" + effect.name + "'");
          return;
        }
      }

      /// Destroys an effect.
      ///
      /// @param effectPtr Pointer to effect.
      public static void Effect_Destroy(ref IntPtr effectPtr) {
        if (Ptr == IntPtr.Zero || effectPtr == IntPtr.Zero) {
          return;
        }
        if (!BarelyEffect_Destroy(effectPtr)) {
          Debug.LogError("Failed to destroy effect");
        }
        effectPtr = IntPtr.Zero;
      }

      /// Returns an effect control.
      ///
      /// @param effectPtr Pointer to effect.
      /// @param controlId Control identifier.
      /// @return Control.
      public static Control Effect_GetControl(IntPtr effectPtr, int controlId) {
        IntPtr control = IntPtr.Zero;
        if (!BarelyEffect_GetControl(effectPtr, controlId, ref control) &&
            effectPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get effect control " + controlId);
        }
        return new Control(control);
      }

      /// Processes effect output samples.
      ///
      /// @param effectPtr Pointer to effect.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Effect_Process(IntPtr effectPtr, float[] outputSamples,
                                        int outputChannelCount) {
        if (_ptr == IntPtr.Zero) {
          return;
        }
        if (BarelyEffect_Process(effectPtr, OutputSamples, outputChannelCount,
                                 outputSamples.Length / outputChannelCount,
                                 AudioSettings.dspTime)) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] *= (float)OutputSamples[i];
          }
        }
      }

      /// Sets effect data.
      ///
      /// @param effectPtr Pointer to effect.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Effect_SetData(IntPtr effectPtr, IntPtr dataPtr, int size) {
        if (!BarelyEffect_SetData(effectPtr, dataPtr, size) && effectPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set effect data");
        }
      }

      /// Creates a new instrument.
      ///
      /// @param instrument Instrument.
      /// @param instrumentPtr Pointer to instrument.
      public static void Instrument_Create(Instrument instrument, ref IntPtr instrumentPtr) {
        if (Ptr == IntPtr.Zero || instrumentPtr != IntPtr.Zero) {
          return;
        }
        InstrumentDefinition definition;
        switch (instrument) {
          case PercussionInstrument percussion:
            definition = BarelyPercussionInstrument_GetDefinition();
            break;
          case SamplerInstrument sampler:
            definition = BarelySamplerInstrument_GetDefinition();
            break;
          case SynthInstrument synth:
            definition = BarelySynthInstrument_GetDefinition();
            break;
          case CustomInstrumentInterface custom:
            definition = custom.GetDefinition();
            break;
          default:
            Debug.LogError("Unsupported instrument type: " + instrument.GetType());
            return;
        }
        bool success = BarelyInstrument_Create(Ptr, definition, ref instrumentPtr);
        if (instrument.GetType().IsSubclassOf(typeof(CustomInstrumentInterface))) {
          if (definition.controlDefinitionCount > 0) {
            Marshal.FreeHGlobal(definition.controlDefinitions);
          }
          if (definition.noteControlDefinitionCount > 0) {
            Marshal.FreeHGlobal(definition.noteControlDefinitions);
          }
        }
        if (!success) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "'");
          return;
        }
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentPtr Pointer to instrument.
      public static void Instrument_Destroy(ref IntPtr instrumentPtr) {
        if (Ptr == IntPtr.Zero || instrumentPtr == IntPtr.Zero) {
          return;
        }
        if (!BarelyInstrument_Destroy(instrumentPtr)) {
          Debug.LogError("Failed to destroy instrument");
        }
        instrumentPtr = IntPtr.Zero;
      }

      /// Returns an instrument control.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param controlId Control identifier.
      /// @return Control.
      public static Control Instrument_GetControl(IntPtr instrumentPtr, int controlId) {
        IntPtr control = IntPtr.Zero;
        if (!BarelyInstrument_GetControl(instrumentPtr, controlId, ref control) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument control " + controlId);
        }
        return new Control(control);
      }

      /// Processes instrument output samples.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Instrument_Process(IntPtr instrumentPtr, float[] outputSamples,
                                            int outputChannelCount) {
        if (_ptr == IntPtr.Zero) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
          return;
        }
        if (BarelyInstrument_Process(instrumentPtr, OutputSamples, outputChannelCount,
                                     outputSamples.Length / outputChannelCount,
                                     AudioSettings.dspTime)) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] *= (float)OutputSamples[i];
          }
        } else {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentPtr Pointer to instrument.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Instrument_SetData(IntPtr instrumentPtr, IntPtr dataPtr, int size) {
        if (!BarelyInstrument_SetData(instrumentPtr, dataPtr, size) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument data");
        }
      }

      /// Returns the corresponding number of seconds for a given number of musician beats.
      ///
      /// @param frames Number of beats.
      /// @return Number of seconds.
      public static double Musician_GetSecondsFromBeats(double beats) {
        double seconds = 0.0;
        if (!BarelyMusician_GetSecondsFromBeats(Ptr, beats, ref seconds) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to get seconds for " + beats + " musician beats");
        }
        return seconds;
      }

      /// Returns the corresponding number of musician beats for a given number of seconds.
      ///
      /// @param frames Number of seconds.
      /// @return Number of beats.
      public static double Musician_GetBeatsFromSeconds(double seconds) {
        double beats = 0.0;
        if (!BarelyMusician_GetBeatsFromSeconds(Ptr, seconds, ref beats) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to get musician beats for " + seconds + " seconds");
        }
        return beats;
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        double tempo = 0.0;
        if (!BarelyMusician_GetTempo(Ptr, ref tempo) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo");
        }
        return tempo;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        double timestamp = 0.0;
        if (!BarelyMusician_GetTimestamp(Ptr, ref timestamp) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to get musician timestamp");
        }
        return timestamp;
      }

      /// Schedules a new musician task.
      ///
      /// @param callback Task process callback.
      /// @param timestamp Task timestamp in seconds.
      public static void Musician_ScheduleTask(Action callback, double timestamp) {
        if (timestamp < Timestamp) {
          Debug.LogError("Failed to create musician task at " + timestamp);
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

      /// Sets the tempo of a musician.
      ///
      /// @param tempo Tempo in beats per minute.
      public static void Musician_SetTempo(double tempo) {
        if (!BarelyMusician_SetTempo(Ptr, tempo) && _ptr != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo");
        }
      }

      /// Returns a note control.
      ///
      /// @param notePtr Pointer to note.
      /// @param controlId Control identifier.
      /// @return Control.
      public static Control Note_GetControl(IntPtr notePtr, int controlId) {
        IntPtr control = IntPtr.Zero;
        if (!BarelyNote_GetControl(notePtr, controlId, ref control) && notePtr != IntPtr.Zero) {
          Debug.LogError("Failed to get note control " + controlId);
          return null;
        }
        return new Control(control);
      }

      public static Note Note_Create(IntPtr instrumentPtr, double pitch, double intensity) {
        IntPtr note = IntPtr.Zero;
        if (!BarelyNote_Create(instrumentPtr, pitch, intensity, ref note) &&
            instrumentPtr != IntPtr.Zero) {
          Debug.LogError("Failed to create note");
          return null;
        }
        return new Note(note);
      }

      public static void Note_Destroy(ref IntPtr notePtr) {
        if (Ptr == IntPtr.Zero || notePtr == IntPtr.Zero) {
          return;
        }
        if (!BarelyNote_Destroy(notePtr)) {
          Debug.LogError("Failed to destroy note");
        }
        notePtr = IntPtr.Zero;
      }

      /// Cancels all one-off tasks.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_CancelAllOneOffTasks(IntPtr performerPtr) {
        if (!BarelyPerformer_CancelAllOneOffTasks(performerPtr) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to cancel all one-off tasks");
        }
      }

      /// Creates a new performer.
      ///
      /// @param performer Performer.
      /// @param performerPtr Pointer to performer.
      public static void Performer_Create(Performer performer, ref IntPtr performerPtr) {
        if (Ptr == IntPtr.Zero || performerPtr != IntPtr.Zero) {
          return;
        }
        if (!BarelyPerformer_Create(Ptr, /*processOrder=*/0, ref performerPtr)) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
        }
      }

      /// Destroys a performer.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_Destroy(ref IntPtr performerPtr) {
        if (Ptr == IntPtr.Zero || performerPtr == IntPtr.Zero) {
          return;
        }
        if (!BarelyPerformer_Destroy(performerPtr)) {
          Debug.LogError("Failed to destroy performer");
        }
        performerPtr = IntPtr.Zero;
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(IntPtr performerPtr) {
        double loopBeginPosition = 0.0;
        if (!BarelyPerformer_GetLoopBeginPosition(performerPtr, ref loopBeginPosition) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop begin position");
        }
        return loopBeginPosition;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(IntPtr performerPtr) {
        double loopLength = 0.0;
        if (!BarelyPerformer_GetLoopLength(performerPtr, ref loopLength) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop length");
        }
        return loopLength;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return Position in beats.
      public static double Performer_GetPosition(IntPtr performerPtr) {
        double position = 0.0;
        if (!BarelyPerformer_GetPosition(performerPtr, ref position) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer position");
        }
        return position;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(IntPtr performerPtr) {
        bool isLooping = false;
        if (!BarelyPerformer_IsLooping(performerPtr, ref isLooping) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is looping");
        }
        return isLooping;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerPtr Pointer to performer.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(IntPtr performerPtr) {
        bool isPlaying = false;
        if (!BarelyPerformer_IsPlaying(performerPtr, ref isPlaying) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if performer is playing");
        }
        return isPlaying;
      }

      /// Schedules a one-off task.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param callback Task callback.
      /// @param position Task position.
      public static void Performer_ScheduleOneOffTask(IntPtr performerPtr, Action callback,
                                                      double position) {
        if (Ptr == null || callback == null) {
          return;
        }
        GCHandle pointer = GCHandle.Alloc(callback);
        if (!BarelyPerformer_ScheduleOneOffTask(performerPtr, _taskDefinition, position,
                                                GCHandle.ToIntPtr(pointer)) &&
            performerPtr != IntPtr.Zero) {
          pointer.Free();
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(IntPtr performerPtr,
                                                        double loopBeginPosition) {
        if (!BarelyPerformer_SetLoopBeginPosition(performerPtr, loopBeginPosition) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(IntPtr performerPtr, double loopLength) {
        if (!BarelyPerformer_SetLoopLength(performerPtr, loopLength) &&
            performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(IntPtr performerPtr, bool isLooping) {
        if (!BarelyPerformer_SetLooping(performerPtr, isLooping) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param position Position in beats.
      public static void Performer_SetPosition(IntPtr performerPtr, double position) {
        if (!BarelyPerformer_SetPosition(performerPtr, position) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer position");
        }
      }

      /// Starts a performer.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_Start(IntPtr performerPtr) {
        if (!BarelyPerformer_Start(performerPtr) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to start performer");
        }
      }

      /// Stops a performer.
      ///
      /// @param performerPtr Pointer to performer.
      public static void Performer_Stop(IntPtr performerPtr) {
        if (!BarelyPerformer_Stop(performerPtr) && performerPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop performer");
        }
      }

      /// Creates a new task.
      ///
      /// @param performerPtr Pointer to performer.
      /// @param callback Task callback.
      /// @param position Task position.
      /// @param taskPtr Pointer to task.
      public static void Task_Create(IntPtr performerPtr, Action callback, double position,
                                     ref IntPtr taskPtr) {
        if (Ptr == IntPtr.Zero || taskPtr != IntPtr.Zero) {
          return;
        }
        GCHandle pointer = GCHandle.Alloc(callback);
        if (!BarelyTask_Create(performerPtr, _taskDefinition, position, GCHandle.ToIntPtr(pointer),
                               ref taskPtr)) {
          pointer.Free();
        }
      }

      /// Destroys a task.
      ///
      /// @param taskPtr Pointer to task.
      public static void Task_Destroy(ref IntPtr taskPtr) {
        if (Ptr == IntPtr.Zero || taskPtr == IntPtr.Zero) {
          return;
        }
        if (!BarelyTask_Destroy(taskPtr)) {
          Debug.LogError("Failed to destroy performer task");
        }
        taskPtr = IntPtr.Zero;
      }

      /// Returns the position of a task.
      ///
      /// @param taskPtr Pointer to task.
      /// @return Position in beats.
      public static double Task_GetPosition(IntPtr taskPtr) {
        double position = 0.0;
        if (!BarelyTask_GetPosition(taskPtr, ref position) && taskPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position");
        }
        return position;
      }

      /// Sets the position of a task.
      ///
      /// @param taskPtr Pointer to task.
      /// @param position Position in beats.
      public static void Task_SetPosition(IntPtr taskPtr, double position) {
        if (!BarelyTask_SetPosition(taskPtr, position) && taskPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      /// Returns whether an arpeggiator note is on or not.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Arpeggiator_IsNoteOn(IntPtr arpeggiatorPtr, double pitch) {
        bool isNoteOn = false;
        if (!BarelyArpeggiator_IsNoteOn(arpeggiatorPtr, pitch, ref isNoteOn) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator note pitch " + pitch + " is on");
        }
        return isNoteOn;
      }

      /// Returns whether an arpeggiator is playing or not.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @return True if playing, false otherwise.
      public static bool Arpeggiator_IsPlaying(IntPtr arpeggiatorPtr) {
        bool isPlaying = false;
        if (!BarelyArpeggiator_IsPlaying(arpeggiatorPtr, ref isPlaying) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator is playing");
        }
        return isPlaying;
      }

      /// Sets all arpeggiator notes off.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      public static void Arpeggiator_SetAllNotesOff(IntPtr arpeggiatorPtr) {
        if (!BarelyArpeggiator_SetAllNotesOff(arpeggiatorPtr) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop all arpeggiator notes");
        }
      }

      /// Sets an arpeggiator gate ratio.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param gateRatio Gate ratio.
      public static void Arpeggiator_SetGateRatio(IntPtr arpeggiatorPtr, double gateRatio) {
        if (!BarelyArpeggiator_SetGateRatio(arpeggiatorPtr, gateRatio) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator gate ratio " + gateRatio);
        }
      }

      /// Sets an arpeggiator instrument.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param instrument Instrument.
      public static void Arpeggiator_SetInstrument(IntPtr arpeggiatorPtr, Instrument instrument) {
        if (!BarelyArpeggiator_SetInstrument(arpeggiatorPtr,
                                             Instrument.Internal.GetPtr(instrument)) &&
            arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator instrument '" + instrument.name + "'");
        }
      }

      /// Sets an arpeggiator note off.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOff(IntPtr arpeggiatorPtr, double pitch) {
        if (!BarelyArpeggiator_SetNoteOff(arpeggiatorPtr, pitch) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator note on.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOn(IntPtr arpeggiatorPtr, double pitch) {
        if (!BarelyArpeggiator_SetNoteOn(arpeggiatorPtr, pitch) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to start arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator rate.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param rate Rate in notes per beat.
      public static void Arpeggiator_SetRate(IntPtr arpeggiatorPtr, double rate) {
        if (!BarelyArpeggiator_SetRate(arpeggiatorPtr, rate) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator rate " + rate);
        }
      }

      /// Sets an arpeggiator style.
      ///
      /// @param arpeggiatorPtr Pointer to arpeggiator.
      /// @param style Style.
      public static void Arpeggiator_SetStyle(IntPtr arpeggiatorPtr, ArpeggiatorStyle style) {
        if (!BarelyArpeggiator_SetStyle(arpeggiatorPtr, style) && arpeggiatorPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set arpeggiator style " + style);
        }
      }

      /// Returns whether an repeater is playing or not.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @return True if playing, false otherwise.
      public static bool Repeater_IsPlaying(IntPtr repeaterPtr) {
        bool isPlaying = false;
        if (!BarelyRepeater_IsPlaying(repeaterPtr, ref isPlaying) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to get if repeater is playing");
        }
        return isPlaying;
      }

      /// Pops the last note from the end.
      ///
      /// @param repeaterPtr Pointer to repeater.
      public static void Repeater_Pop(IntPtr repeaterPtr) {
        if (!BarelyRepeater_Pop(repeaterPtr) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Pushes a new note to the end.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param pitchOr Note pitch or silence.
      /// @param length Note length in beats.
      public static void Repeater_Push(IntPtr repeaterPtr, double? pitchOr, int length) {
        if ((pitchOr.HasValue && !BarelyRepeater_Push(repeaterPtr, pitchOr.Value, length)) ||
            (!pitchOr.HasValue && !BarelyRepeater_PushSilence(repeaterPtr, length)) &&
                repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to pop from repeater");
        }
      }

      /// Sets an repeater instrument.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param instrument Instrument.
      public static void Repeater_SetInstrument(IntPtr repeaterPtr, Instrument instrument) {
        if (!BarelyRepeater_SetInstrument(repeaterPtr, Instrument.Internal.GetPtr(instrument)) &&
            repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater instrument '" + instrument.name + "'");
        }
      }

      /// Sets an repeater rate.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param rate Rate in notes per beat.
      public static void Repeater_SetRate(IntPtr repeaterPtr, double rate) {
        if (!BarelyRepeater_SetRate(repeaterPtr, rate) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater rate " + rate);
        }
      }

      /// Sets an repeater style.
      ///
      /// @param repeaterPtr Pointer to repeater.
      /// @param style Style.
      public static void Repeater_SetStyle(IntPtr repeaterPtr, RepeaterStyle style) {
        if (!BarelyRepeater_SetStyle(repeaterPtr, style) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to set repeater style " + style);
        }
      }

      /// Starts a repeater.
      ///
      /// @param repeaterPtr Pointer to repeater.
      public static void Repeater_Start(IntPtr repeaterPtr, double pitchOffset) {
        if (!BarelyRepeater_Start(repeaterPtr, pitchOffset) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to start repeater with pitch offset " + pitchOffset);
        }
      }

      /// Stops a repeater.
      ///
      /// @param repeaterPtr Pointer to repeater.
      public static void Repeater_Stop(IntPtr repeaterPtr) {
        if (!BarelyRepeater_Stop(repeaterPtr) && repeaterPtr != IntPtr.Zero) {
          Debug.LogError("Failed to stop repeater");
        }
      }

      // Internal output samples.
      public static double[] OutputSamples { get; private set; } = null;

      // Task definition create callback.
      private delegate void TaskDefinition_CreateCallback(ref IntPtr state, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_CreateCallback))]
      private static void TaskDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Task definition destroy callback.
      private delegate void TaskDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_DestroyCallback))]
      private static void TaskDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Task definition process callback.
      private delegate void TaskDefinition_ProcessCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_ProcessCallback))]
      private static void TaskDefinition_OnProcess(ref IntPtr state) {
        (GCHandle.FromIntPtr(state).Target as Action)?.Invoke();
      }

      // Task definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct TaskDefinition {
        // Create callback.
        public TaskDefinition_CreateCallback createCallback;

        // Destroy callback.
        public TaskDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public TaskDefinition_ProcessCallback processCallback;
      }

      // Singleton musician pointer.
      private static IntPtr Ptr {
        get {
          if (_isShuttingDown) {
            return IntPtr.Zero;
          }
          if (_ptr == IntPtr.Zero) {
            var state =
                new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<State>();
            GameObject.DontDestroyOnLoad(state.gameObject);
            if (_ptr == IntPtr.Zero) {
              GameObject.DestroyImmediate(state.gameObject);
              _isShuttingDown = true;
            }
          }
          return _ptr;
        }
      }
      private static IntPtr _ptr = IntPtr.Zero;

      // Task definition.
      private static TaskDefinition _taskDefinition = new TaskDefinition() {
        createCallback = TaskDefinition_OnCreate,
        destroyCallback = TaskDefinition_OnDestroy,
        processCallback = TaskDefinition_OnProcess,
      };

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Latency in seconds.
      private static double _latency = 0.0;

      // Map of scheduled list of task callbacks by their timestamps.
      private static SortedDictionary<double, List<Action>> _scheduledTaskCallbacks = null;

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
          var instruments = FindObjectsOfType<Instrument>();
          for (int i = 0; i < instruments.Length; ++i) {
            instruments[i].enabled = false;
          }
          Initialize();
          for (int i = 0; i < instruments.Length; ++i) {
            instruments[i].enabled = true;
          }
        }

        private void LateUpdate() {
          double nextTimestamp = GetNextTimestamp();
          while (_scheduledTaskCallbacks.Count > 0) {
            double taskTimestamp = _scheduledTaskCallbacks.ElementAt(0).Key;
            if (taskTimestamp > nextTimestamp) {
              break;
            }
            BarelyMusician_Update(_ptr, taskTimestamp);
            var callbacks = _scheduledTaskCallbacks.ElementAt(0).Value;
            for (int i = 0; i < callbacks.Count; ++i) {
              callbacks[i]?.Invoke();
            }
            _scheduledTaskCallbacks.Remove(taskTimestamp);
          }
          BarelyMusician_Update(_ptr, nextTimestamp);
        }

        // Initializes the native state.
        private void Initialize() {
          _isShuttingDown = false;
          var config = AudioSettings.GetConfiguration();
          if (!BarelyMusician_Create(config.sampleRate, ref _ptr)) {
            Debug.LogError("Failed to initialize BarelyMusician");
            return;
          }
          BarelyMusician_SetTempo(_ptr, _tempo);
          OutputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _latency = (double)config.dspBufferSize / config.sampleRate;
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          BarelyMusician_Update(_ptr, GetNextTimestamp());
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_ptr);
          _ptr = IntPtr.Zero;
          _scheduledTaskCallbacks = null;
        }

        // Returns the next timestamp to update.
        private double GetNextTimestamp() {
          double lookahead = Math.Max(_latency, (double)Time.smoothDeltaTime);
          return AudioSettings.dspTime + lookahead;
        }
      }

#if !UNITY_EDITOR && UNITY_IOS
      private const string pluginName = "__Internal";
#else
      private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

      [DllImport(pluginName, EntryPoint = "BarelyControl_GetValue")]
      private static extern bool BarelyControl_GetValue(IntPtr control, ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyControl_ResetValue")]
      private static extern bool BarelyControl_ResetValue(IntPtr control);

      [DllImport(pluginName, EntryPoint = "BarelyControl_SetValue")]
      private static extern bool BarelyControl_SetValue(IntPtr control, double value);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_Create")]
      private static extern bool BarelyEffect_Create(IntPtr musician, EffectDefinition definition,
                                                     ref IntPtr outEffect);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_Destroy")]
      private static extern bool BarelyEffect_Destroy(IntPtr effect);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_GetControl")]
      private static extern bool BarelyEffect_GetControl(IntPtr effect, Int32 controlId,
                                                         ref IntPtr outControl);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_Process")]
      private static extern bool BarelyEffect_Process(IntPtr effect,
                                                      [In, Out] double[] outputSamples,
                                                      Int32 outputChannelCount,
                                                      Int32 outputFrameCount, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_SetData")]
      private static extern bool BarelyEffect_SetData(IntPtr effect, IntPtr data, Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Create")]
      private static extern bool BarelyInstrument_Create(IntPtr musician,
                                                         InstrumentDefinition definition,
                                                         ref IntPtr outInstrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern bool BarelyInstrument_Destroy(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern bool BarelyInstrument_GetControl(IntPtr instrument, Int32 controlId,
                                                             ref IntPtr outControl);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern bool BarelyInstrument_Process(IntPtr instrument,
                                                          [In, Out] double[] outputSamples,
                                                          Int32 outputChannelCount,
                                                          Int32 outputFrameCount, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
      private static extern bool BarelyInstrument_SetData(IntPtr instrument, IntPtr data,
                                                          Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
      private static extern bool BarelyMusician_Create(Int32 frameRate, ref IntPtr outMusician);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
      private static extern bool BarelyMusician_Destroy(IntPtr musician);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetBeatsFromSeconds")]
      private static extern bool BarelyMusician_GetBeatsFromSeconds(IntPtr musician, double seconds,
                                                                    ref double outBeats);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetSecondsFromBeats")]
      private static extern bool BarelyMusician_GetSecondsFromBeats(IntPtr musician, double beats,
                                                                    ref double outSeconds);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
      private static extern bool BarelyMusician_GetTempo(IntPtr musician, ref double outTempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
      private static extern bool BarelyMusician_GetTimestamp(IntPtr musician,
                                                             ref double outTimestamp);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern bool BarelyMusician_SetTempo(IntPtr musician, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern bool BarelyMusician_Update(IntPtr musician, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyNote_Create")]
      private static extern bool BarelyNote_Create(IntPtr instrument, double pitch,
                                                   double intensity, ref IntPtr outNote);

      [DllImport(pluginName, EntryPoint = "BarelyNote_Destroy")]
      private static extern bool BarelyNote_Destroy(IntPtr note);

      [DllImport(pluginName, EntryPoint = "BarelyNote_GetControl")]
      private static extern bool BarelyNote_GetControl(IntPtr note, Int32 controlId,
                                                       ref IntPtr outControl);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CancelAllOneOffTasks")]
      private static extern bool BarelyPerformer_CancelAllOneOffTasks(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Create")]
      private static extern bool BarelyPerformer_Create(IntPtr musician, Int32 processOrder,
                                                        ref IntPtr outPerformer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Destroy")]
      private static extern bool BarelyPerformer_Destroy(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopBeginPosition")]
      private static extern bool BarelyPerformer_GetLoopBeginPosition(
          IntPtr performer, ref double outLoopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopLength")]
      private static extern bool BarelyPerformer_GetLoopLength(IntPtr performer,
                                                               ref double outLoopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern bool BarelyPerformer_GetPosition(IntPtr performer,
                                                             ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsLooping")]
      private static extern bool BarelyPerformer_IsLooping(IntPtr performer, ref bool outIsLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsPlaying")]
      private static extern bool BarelyPerformer_IsPlaying(IntPtr performer, ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_ScheduleOneOffTask")]
      private static extern bool BarelyPerformer_ScheduleOneOffTask(IntPtr performer,
                                                                    TaskDefinition definition,
                                                                    double position,
                                                                    IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLoopBeginPosition")]
      private static extern bool BarelyPerformer_SetLoopBeginPosition(IntPtr performer,
                                                                      double loopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLoopLength")]
      private static extern bool BarelyPerformer_SetLoopLength(IntPtr performer, double loopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLooping")]
      private static extern bool BarelyPerformer_SetLooping(IntPtr performer, bool isLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetPosition")]
      private static extern bool BarelyPerformer_SetPosition(IntPtr performer, double position);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Start")]
      private static extern bool BarelyPerformer_Start(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Stop")]
      private static extern bool BarelyPerformer_Stop(IntPtr performer);

      [DllImport(pluginName, EntryPoint = "BarelyTask_Create")]
      private static extern bool BarelyTask_Create(IntPtr performer, TaskDefinition definition,
                                                   double position, IntPtr userData,
                                                   ref IntPtr outTask);

      [DllImport(pluginName, EntryPoint = "BarelyTask_Destroy")]
      private static extern bool BarelyTask_Destroy(IntPtr task);

      [DllImport(pluginName, EntryPoint = "BarelyTask_GetPosition")]
      private static extern bool BarelyTask_GetPosition(IntPtr task, ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyTask_SetPosition")]
      private static extern bool BarelyTask_SetPosition(IntPtr task, double position);

      // Components.
      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_Create")]
      private static extern bool BarelyArpeggiator_Create(IntPtr musician, Int32 processOrder,
                                                          ref IntPtr outArpeggiator);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_Destroy")]
      private static extern bool BarelyArpeggiator_Destroy(IntPtr arpeggiator);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_IsNoteOn")]
      private static extern bool BarelyArpeggiator_IsNoteOn(IntPtr arpeggiator, double pitch,
                                                            ref bool outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_IsPlaying")]
      private static extern bool BarelyArpeggiator_IsPlaying(IntPtr arpeggiator,
                                                             ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetAllNotesOff")]
      private static extern bool BarelyArpeggiator_SetAllNotesOff(IntPtr arpeggiator);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetGateRatio")]
      private static extern bool BarelyArpeggiator_SetGateRatio(IntPtr arpeggiator,
                                                                double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetInstrument")]
      private static extern bool BarelyArpeggiator_SetInstrument(IntPtr arpeggiator,
                                                                 IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetNoteOff")]
      private static extern bool BarelyArpeggiator_SetNoteOff(IntPtr arpeggiator, double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetNoteOn")]
      private static extern bool BarelyArpeggiator_SetNoteOn(IntPtr arpeggiator, double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetRate")]
      private static extern bool BarelyArpeggiator_SetRate(IntPtr arpeggiator, double rate);

      [DllImport(pluginName, EntryPoint = "BarelyArpeggiator_SetStyle")]
      private static extern bool BarelyArpeggiator_SetStyle(IntPtr arpeggiator,
                                                            ArpeggiatorStyle style);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Clear")]
      private static extern bool BarelyRepeater_Clear(IntPtr repeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Create")]
      private static extern bool BarelyRepeater_Create(IntPtr musician, Int32 processOrder,
                                                       ref IntPtr outRepeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Destroy")]
      private static extern bool BarelyRepeater_Destroy(IntPtr repeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_IsPlaying")]
      private static extern bool BarelyRepeater_IsPlaying(IntPtr repeater, ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Pop")]
      private static extern bool BarelyRepeater_Pop(IntPtr repeater);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Push")]
      private static extern bool BarelyRepeater_Push(IntPtr repeater, double pitch, Int32 length);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_PushSilence")]
      private static extern bool BarelyRepeater_PushSilence(IntPtr repeater, Int32 length);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetGateRatio")]
      private static extern bool BarelyRepeater_SetGateRatio(IntPtr repeater, double gateRatio);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetInstrument")]
      private static extern bool BarelyRepeater_SetInstrument(IntPtr repeater, IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetRate")]
      private static extern bool BarelyRepeater_SetRate(IntPtr repeater, double rate);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_SetStyle")]
      private static extern bool BarelyRepeater_SetStyle(IntPtr repeater, RepeaterStyle style);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Start")]
      private static extern bool BarelyRepeater_Start(IntPtr repeater, double pitchShift);

      [DllImport(pluginName, EntryPoint = "BarelyRepeater_Stop")]
      private static extern bool BarelyRepeater_Stop(IntPtr repeater);

      // Effects.
      [DllImport(pluginName, EntryPoint = "BarelyHighPassEffect_GetDefinition")]
      private static extern EffectDefinition BarelyHighPassEffect_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelyLfoEffect_GetDefinition")]
      private static extern EffectDefinition BarelyLfoEffect_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelyLowPassEffect_GetDefinition")]
      private static extern EffectDefinition BarelyLowPassEffect_GetDefinition();

      // Instruments.
      [DllImport(pluginName, EntryPoint = "BarelyPercussionInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelyPercussionInstrument_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySamplerInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySamplerInstrument_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySynthInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySynthInstrument_GetDefinition();
    }
  }
}  // namespace Barely
