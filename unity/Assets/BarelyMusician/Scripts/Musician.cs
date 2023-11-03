using System;
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

    /// Returns the corresponding midi key for a given `pitch`.
    ///
    /// @param pitch Note pitch.
    /// @return Midi key.
    public static int MidiKeyFromPitch(double pitch) {
      return 69 + (int)(pitch * 12.0);
    }

    /// Returns the corresponding pitch for a given `midiKey`.
    /// @param midiKey Midi key.
    /// @param Note pitch.
    public static double PitchFromMidiKey(int midiKey) {
      return (double)(midiKey - 69) / 12.0;
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
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerFrame Control slope in value change per frame.
      public delegate void EffectDefinition_SetControlCallback(ref IntPtr state, Int32 index,
                                                               double value, double slopePerFrame);

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
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerFrame Control slope in value change per frame.
      public delegate void InstrumentDefinition_SetControlCallback(ref IntPtr state, Int32 index,
                                                                   double value,
                                                                   double slopePerFrame);

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
      /// @param index Note control index.
      /// @param value Note control value.
      /// @param slopePerFrame Note control slope in value change per frame.
      public delegate void InstrumentDefinition_SetNoteControlCallback(ref IntPtr state,
                                                                       double pitch, Int32 index,
                                                                       double value,
                                                                       double slopePerFrame);

      /// Instrument definition set note off callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param pitch Note pitch.
      public delegate void InstrumentDefinition_SetNoteOffCallback(ref IntPtr state, double pitch);

      /// Instrument definition set note on callback signature.
      ///
      /// @param state Pointer to instrument state.
      /// @param pitch Note pitch.
      /// @param intensity Note intensity.
      public delegate void InstrumentDefinition_SetNoteOnCallback(ref IntPtr state, double pitch,
                                                                  double intensity);

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

      /// Creates a new effect.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param effect Effect.
      /// @param effectHandle Effect handle.
      public static void Effect_Create(IntPtr instrumentHandle, Effect effect,
                                       ref IntPtr effectHandle) {
        if (Handle == IntPtr.Zero || instrumentHandle == IntPtr.Zero ||
            effectHandle != IntPtr.Zero) {
          return;
        }
        EffectDefinition definition;
        switch (effect) {
          case HighPassEffect highPass:
            definition = BarelyHighPassEffectDefinition();
            break;
          case LowPassEffect lowPass:
            definition = BarelyLowPassEffectDefinition();
            break;
          case CustomEffectInterface custom:
            definition = custom.GetDefinition();
            break;
          default:
            Debug.LogError("Unsupported effect type: " + effect.GetType());
            return;
        }
        bool success = BarelyEffect_Create(instrumentHandle, definition, effect.ProcessOrder,
                                           ref effectHandle);
        if (effect.GetType().IsSubclassOf(typeof(CustomEffectInterface))) {
          if (definition.controlDefinitionCount > 0) {
            Marshal.FreeHGlobal(definition.controlDefinitions);
          }
        }
        if (!success) {
          Debug.LogError("Failed to create effect '" + effect.name + "'");
          return;
        }
        _effects?.Add(effectHandle, effect);
        BarelyEffect_SetControlEvent(effectHandle, _effectControlEventDefinition, effectHandle);
      }

      /// Destroys an effect.
      ///
      /// @param effectHandle Effect handle.
      public static void Effect_Destroy(ref IntPtr effectHandle) {
        if (Handle == IntPtr.Zero || effectHandle == IntPtr.Zero) {
          return;
        }
        bool success = BarelyEffect_Destroy(effectHandle);
        if (!success && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy effect");
        }
        _effects?.Remove(effectHandle);
        effectHandle = IntPtr.Zero;
      }

      /// Returns the value of an effect control.
      ///
      /// @param effectHandle Effect handle.
      /// @param index Control index.
      /// @return Control value.
      public static double Effect_GetControl(IntPtr effectHandle, int index) {
        double value = 0.0;
        bool success = BarelyEffect_GetControl(effectHandle, index, ref value);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get effect control " + index + " value");
        }
        return value;
      }

      /// Returns the process order of an effect.
      ///
      /// @param effectHandle Effect handle.
      /// @return Process order.
      public static int Effect_GetProcessOrder(IntPtr effectHandle) {
        Int32 processOrder = 0;
        bool success = BarelyEffect_GetProcessOrder(effectHandle, ref processOrder);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get effect process order");
        }
        return processOrder;
      }

      /// Resets all effect control values.
      ///
      /// @param effectHandle Effect handle.
      public static void Effect_ResetAllControls(IntPtr effectHandle) {
        bool success = BarelyEffect_ResetAllControls(effectHandle);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all effect controls");
        }
      }

      /// Resets an effect control value.
      ///
      /// @param effectHandle Effect handle.
      /// @param index Control index.
      public static void Effect_ResetControl(IntPtr effectHandle, int index) {
        bool success = BarelyEffect_ResetControl(effectHandle, index);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset effect control " + index + " value");
        }
      }

      /// Sets an effect control value.
      ///
      /// @param effectHandle Effect handle.
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerBeat Control slope in value change per beat.
      public static void Effect_SetControl(IntPtr effectHandle, int index, double value,
                                           double slopePerBeat) {
        bool success = BarelyEffect_SetControl(effectHandle, index, value, slopePerBeat);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set effect control " + index + " value to " + value +
                         " with slope " + slopePerBeat + "");
        }
      }

      /// Sets effect data.
      ///
      /// @param effectHandle Effect handle.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Effect_SetData(IntPtr effectHandle, IntPtr dataPtr, int size) {
        bool success = BarelyEffect_SetData(effectHandle, dataPtr, size);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set effect data");
        }
      }

      /// Sets the process order of an effect.
      ///
      /// @param effectHandle Effect handle.
      /// @param processOrder Process order.
      public static void Effect_SetProcessOrder(IntPtr effectHandle, int processOrder) {
        bool success = BarelyEffect_SetProcessOrder(effectHandle, processOrder);
        if (!success && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set effect process order to" + processOrder + "");
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
        InstrumentDefinition definition;
        switch (instrument) {
          case PercussionInstrument percussion:
            definition = BarelyPercussionInstrumentDefinition();
            break;
          case SamplerInstrument sampler:
            definition = BarelySamplerInstrumentDefinition();
            break;
          case SynthInstrument synth:
            definition = BarelySynthInstrumentDefinition();
            break;
          case CustomInstrumentInterface custom:
            definition = custom.GetDefinition();
            break;
          default:
            Debug.LogError("Unsupported instrument type: " + instrument.GetType());
            return;
        }
        bool success = BarelyInstrument_Create(Handle, definition, AudioSettings.outputSampleRate,
                                               ref instrumentHandle);
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
        _instruments?.Add(instrumentHandle, instrument);
        BarelyInstrument_SetControlEvent(instrumentHandle, _instrumentControlEventDefinition,
                                         instrumentHandle);
        BarelyInstrument_SetNoteControlEvent(instrumentHandle, _noteControlEventDefinition,
                                             instrumentHandle);
        BarelyInstrument_SetNoteOffEvent(instrumentHandle, _noteOffEventDefinition,
                                         instrumentHandle);
        BarelyInstrument_SetNoteOnEvent(instrumentHandle, _noteOnEventDefinition, instrumentHandle);
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_Destroy(ref IntPtr instrumentHandle) {
        if (Handle == IntPtr.Zero || instrumentHandle == IntPtr.Zero) {
          return;
        }
        bool success = BarelyInstrument_Destroy(instrumentHandle);
        if (!success) {
          Debug.LogError("Failed to destroy instrument");
        }
        _instruments?.Remove(instrumentHandle);
        instrumentHandle = IntPtr.Zero;
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param index Control index.
      /// @return Control value.
      public static double Instrument_GetControl(IntPtr instrumentHandle, int index) {
        double value = 0.0;
        bool success = BarelyInstrument_GetControl(instrumentHandle, index, ref value);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument control " + index + " value");
        }
        return value;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @return Note control value.
      public static double Instrument_GetNoteControl(IntPtr instrumentHandle, double pitch,
                                                     int index) {
        double value = 0.0;
        bool success = BarelyInstrument_GetNoteControl(instrumentHandle, pitch, index, ref value);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument note pitch " + pitch + " control " + index +
                         " value");
        }
        return value;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(IntPtr instrumentHandle, double pitch) {
        bool isNoteOn = false;
        bool success = BarelyInstrument_IsNoteOn(instrumentHandle, pitch, ref isNoteOn);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if instrument note pitch " + pitch + " is on");
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
        bool success = BarelyInstrument_Process(
            instrumentHandle, _outputSamples, outputChannelCount,
            outputSamples.Length / outputChannelCount, AudioSettings.dspTime);
        if (success) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] *= (float)_outputSamples[i];
          }
        } else {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
        }
      }

      /// Resets all instrument control values.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_ResetAllControls(IntPtr instrumentHandle) {
        bool success = BarelyInstrument_ResetAllControls(instrumentHandle);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument controls");
        }
      }

      /// Resets all instrument note control values.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_ResetAllNoteControls(IntPtr instrumentHandle, double pitch) {
        bool success = BarelyInstrument_ResetAllNoteControls(instrumentHandle, pitch);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument note pitch " + pitch + " controls");
        }
      }

      /// Resets an instrument control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param index Control index.
      public static void Instrument_ResetControl(IntPtr instrumentHandle, int index) {
        bool success = BarelyInstrument_ResetControl(instrumentHandle, index);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument control " + index + " value");
        }
      }

      /// Resets an instrument note control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      public static void Instrument_ResetNoteControl(IntPtr instrumentHandle, double pitch,
                                                     int index) {
        bool success = BarelyInstrument_ResetNoteControl(instrumentHandle, pitch, index);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument note pitch " + pitch + " control " + index +
                         " value");
        }
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @return True if success, false otherwise.
      public static void Instrument_SetAllNotesOff(IntPtr instrumentHandle) {
        bool success = BarelyInstrument_SetAllNotesOff(instrumentHandle);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes");
        }
      }

      /// Sets an instrument control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerBeat Control slope in value change per beat.
      public static void Instrument_SetControl(IntPtr instrumentHandle, int index, double value,
                                               double slopePerBeat) {
        bool success = BarelyInstrument_SetControl(instrumentHandle, index, value, slopePerBeat);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument control " + index + " value to " + value +
                         " with slope " + slopePerBeat + "");
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Instrument_SetData(IntPtr instrumentHandle, IntPtr dataPtr, int size) {
        bool success = BarelyInstrument_SetData(instrumentHandle, dataPtr, size);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument data");
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @param value Note control value.
      /// @param slopePerBeat Note control slope in value change per beat.
      public static void Instrument_SetNoteControl(IntPtr instrumentHandle, double pitch, int index,
                                                   double value, double slopePerBeat) {
        bool success =
            BarelyInstrument_SetNoteControl(instrumentHandle, pitch, index, value, slopePerBeat);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument note pitch " + pitch + " control " + index +
                         " value to " + value + " with slope " + slopePerBeat + "");
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @return True if success, false otherwise.
      public static void Instrument_SetNoteOff(IntPtr instrumentHandle, double pitch) {
        bool success = BarelyInstrument_SetNoteOff(instrumentHandle, pitch);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + "");
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOn(IntPtr instrumentHandle, double pitch,
                                              double intensity) {
        bool success = BarelyInstrument_SetNoteOn(instrumentHandle, pitch, intensity);
        if (!success && instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity");
        }
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        double tempo = 0.0;
        bool success = BarelyMusician_GetTempo(Handle, ref tempo);
        if (!success && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo");
        }
        return tempo;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        double timestamp = 0.0;
        bool success = BarelyMusician_GetTimestamp(Handle, ref timestamp);
        if (!success && _handle != IntPtr.Zero) {
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
        bool success = BarelyMusician_SetTempo(Handle, tempo);
        if (!success && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo");
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
        bool success = BarelyPerformer_Create(Handle, ref performerHandle);
        if (!success) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
        }
      }

      /// Destroys a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Destroy(ref IntPtr performerHandle) {
        if (Handle == IntPtr.Zero || performerHandle == IntPtr.Zero) {
          return;
        }
        bool success = BarelyPerformer_Destroy(performerHandle);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy performer");
        }
        performerHandle = IntPtr.Zero;
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(IntPtr performerHandle) {
        double loopBeginPosition = 0.0;
        bool success = BarelyPerformer_GetLoopBeginPosition(performerHandle, ref loopBeginPosition);
        if (!success && performerHandle != IntPtr.Zero) {
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
        bool success = BarelyPerformer_GetLoopLength(performerHandle, ref loopLength);
        if (!success && performerHandle != IntPtr.Zero) {
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
        bool success = BarelyPerformer_GetPosition(performerHandle, ref position);
        if (!success && performerHandle != IntPtr.Zero) {
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
        bool success = BarelyPerformer_IsLooping(performerHandle, ref isLooping);
        if (!success && performerHandle != IntPtr.Zero) {
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
        bool success = BarelyPerformer_IsPlaying(performerHandle, ref isPlaying);
        if (!success && performerHandle != IntPtr.Zero) {
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
        bool success = BarelyPerformer_SetLoopBeginPosition(performerHandle, loopBeginPosition);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(IntPtr performerHandle, double loopLength) {
        bool success = BarelyPerformer_SetLoopLength(performerHandle, loopLength);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerHandle Performer handle.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(IntPtr performerHandle, bool isLooping) {
        bool success = BarelyPerformer_SetLooping(performerHandle, isLooping);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerHandle Performer handle.
      /// @param position Position in beats.
      public static void Performer_SetPosition(IntPtr performerHandle, double position) {
        bool success = BarelyPerformer_SetPosition(performerHandle, position);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer position");
        }
      }

      /// Starts a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Start(IntPtr performerHandle) {
        bool success = BarelyPerformer_Start(performerHandle);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start performer");
        }
      }

      /// Stops a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Stop(IntPtr performerHandle) {
        bool success = BarelyPerformer_Stop(performerHandle);
        if (!success && performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop performer");
        }
      }

      /// Creates a new task.
      ///
      /// @param performerHandle Performer handle.
      /// @param callback Task callback.
      /// @param isOneOff True if one off task, false otherwise.
      /// @param position Task position.
      /// @param processOrder Task process order.
      /// @param taskHandle Task handle.
      public static void Task_Create(IntPtr performerHandle, Action callback, bool isOneOff,
                                     double position, int processOrder, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle != IntPtr.Zero) {
          return;
        }
        IntPtr taskHandlePtr = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        bool success = BarelyTask_Create(performerHandle, _taskDefinition, isOneOff, position,
                                         processOrder, taskHandlePtr, ref taskHandle);
        if (!success) {
          Marshal.DestroyStructure<IntPtr>(taskHandlePtr);
          if (performerHandle != IntPtr.Zero) {
            Debug.LogError("Failed to create performer task");
            return;
          }
        }
        Marshal.WriteIntPtr(taskHandlePtr, taskHandle);
        _taskCallbacks.Add(taskHandle, callback);
      }

      /// Destroys a task.
      ///
      /// @param taskHandle Task handle.
      public static void Task_Destroy(IntPtr performerHandle, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle == IntPtr.Zero) {
          return;
        }
        bool success = BarelyTask_Destroy(taskHandle);
        if (!success && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy performer task");
        }
        taskHandle = IntPtr.Zero;
        _taskCallbacks.Remove(taskHandle);
      }

      /// Returns the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Position in beats.
      public static double Task_GetPosition(IntPtr taskHandle) {
        double position = 0.0;
        bool success = BarelyTask_GetPosition(taskHandle, ref position);
        if (!success && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position");
        }
        return position;
      }

      /// Returns the process order of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Process order.
      public static int Task_GetProcessOrder(IntPtr taskHandle) {
        Int32 processOrder = 0;
        bool success = BarelyTask_GetProcessOrder(taskHandle, ref processOrder);
        if (!success && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task process order");
        }
        return processOrder;
      }

      /// Sets the position of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param position Position in beats.
      public static void Task_SetPosition(IntPtr taskHandle, double position) {
        bool success = BarelyTask_SetPosition(taskHandle, position);
        if (!success && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      /// Sets the process order of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param processOrder Process order.
      public static void Task_SetProcessOrder(IntPtr taskHandle, int processOrder) {
        bool success = BarelyTask_SetProcessOrder(taskHandle, processOrder);
        if (!success && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task process order");
        }
      }

      // Control event definition create callback.
      private delegate void ControlEventDefinition_CreateCallback(ref IntPtr state,
                                                                  IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_CreateCallback))]
      private static void ControlEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        Marshal.WriteIntPtr(state, userData);
      }

      // Control event definition destroy callback.
      private delegate void ControlEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_DestroyCallback))]
      private static void ControlEventDefinition_OnDestroy(ref IntPtr state) {
        Marshal.DestroyStructure<IntPtr>(state);
      }

      // Control event definition process callback.
      private delegate void ControlEventDefinition_ProcessCallback(ref IntPtr state, int index,
                                                                   double value);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_ProcessCallback))]
      private static void EffectControlEventDefinition_OnProcess(ref IntPtr state, int index,
                                                                 double value) {
        Effect effect = null;
        if (_effects.TryGetValue(Marshal.PtrToStructure<IntPtr>(state), out effect)) {
          Effect.Internal.OnControlEvent(effect, index, value);
        }
      }
      private static void InstrumentControlEventDefinition_OnProcess(ref IntPtr state, int index,
                                                                     double value) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(Marshal.PtrToStructure<IntPtr>(state), out instrument)) {
          Instrument.Internal.OnControlEvent(instrument, index, value);
        }
      }

      // Control event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct ControlEventDefinition {
        // Create callback.
        public ControlEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public ControlEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public ControlEventDefinition_ProcessCallback processCallback;
      }

      // Note control event definition create callback.
      private delegate void NoteControlEventDefinition_CreateCallback(ref IntPtr state,
                                                                      IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_CreateCallback))]
      private static void NoteControlEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        Marshal.WriteIntPtr(state, userData);
      }

      // Note control event definition destroy callback.
      private delegate void NoteControlEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_DestroyCallback))]
      private static void NoteControlEventDefinition_OnDestroy(ref IntPtr state) {
        Marshal.DestroyStructure<IntPtr>(state);
      }

      // Note control event definition process callback.
      private delegate void NoteControlEventDefinition_ProcessCallback(ref IntPtr state,
                                                                       double pitch, int index,
                                                                       double value);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_ProcessCallback))]
      private static void NoteControlEventDefinition_OnProcess(ref IntPtr state, double pitch,
                                                               int index, double value) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(Marshal.PtrToStructure<IntPtr>(state), out instrument)) {
          Instrument.Internal.OnNoteControlEvent(instrument, pitch, index, value);
        }
      }

      // Note control event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteControlEventDefinition {
        // Create callback.
        public NoteControlEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public NoteControlEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public NoteControlEventDefinition_ProcessCallback processCallback;
      }

      // Note off event definition create callback.
      private delegate void NoteOffEventDefinition_CreateCallback(ref IntPtr state,
                                                                  IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_CreateCallback))]
      private static void NoteOffEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        Marshal.WriteIntPtr(state, userData);
      }

      // Note off event definition destroy callback.
      private delegate void NoteOffEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_DestroyCallback))]
      private static void NoteOffEventDefinition_OnDestroy(ref IntPtr state) {
        Marshal.DestroyStructure<IntPtr>(state);
      }

      // Note off event definition process callback.
      private delegate void NoteOffEventDefinition_ProcessCallback(ref IntPtr state, double pitch);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_ProcessCallback))]
      private static void NoteOffEventDefinition_OnProcess(ref IntPtr state, double pitch) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(Marshal.PtrToStructure<IntPtr>(state), out instrument)) {
          Instrument.Internal.OnNoteOffEvent(instrument, pitch);
        }
      }

      // Note off event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteOffEventDefinition {
        // Create callback.
        public NoteOffEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public NoteOffEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public NoteOffEventDefinition_ProcessCallback processCallback;
      }

      // Note on event definition create callback.
      private delegate void NoteOnEventDefinition_CreateCallback(ref IntPtr state, IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_CreateCallback))]
      private static void NoteOnEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        Marshal.WriteIntPtr(state, userData);
      }

      // Note on event definition destroy callback.
      private delegate void NoteOnEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_DestroyCallback))]
      private static void NoteOnEventDefinition_OnDestroy(ref IntPtr state) {
        Marshal.DestroyStructure<IntPtr>(state);
      }

      // Note on event definition process callback.
      private delegate void NoteOnEventDefinition_ProcessCallback(ref IntPtr state, double pitch,
                                                                  double intensity);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_ProcessCallback))]
      private static void NoteOnEventDefinition_OnProcess(ref IntPtr state, double pitch,
                                                          double intensity) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(Marshal.PtrToStructure<IntPtr>(state), out instrument)) {
          Instrument.Internal.OnNoteOnEvent(instrument, pitch, intensity);
        }
      }

      // Note on event definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct NoteOnEventDefinition {
        // Create callback.
        public NoteOnEventDefinition_CreateCallback createCallback;

        // Destroy callback.
        public NoteOnEventDefinition_DestroyCallback destroyCallback;

        // Process callback.
        public NoteOnEventDefinition_ProcessCallback processCallback;
      }

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
        if (state != IntPtr.Zero) {
          _taskCallbacks?.Remove(Marshal.PtrToStructure<IntPtr>(state));
          Marshal.DestroyStructure<IntPtr>(state);
        }
      }

      // Task definition process callback.
      private delegate void TaskDefinition_ProcessCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_ProcessCallback))]
      private static void TaskDefinition_OnProcess(ref IntPtr state) {
        Action callback = null;
        if (_taskCallbacks.TryGetValue(Marshal.PtrToStructure<IntPtr>(state), out callback)) {
          callback?.Invoke();
        }
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

      // Singleton musician handle.
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
            }
          }
          return _handle;
        }
      }
      private static IntPtr _handle = IntPtr.Zero;

      // Effect control event definition.
      private static ControlEventDefinition _effectControlEventDefinition =
          new ControlEventDefinition() {
            createCallback = ControlEventDefinition_OnCreate,
            destroyCallback = ControlEventDefinition_OnDestroy,
            processCallback = EffectControlEventDefinition_OnProcess,
          };

      // Instrument control event definition.
      private static ControlEventDefinition _instrumentControlEventDefinition =
          new ControlEventDefinition() {
            createCallback = ControlEventDefinition_OnCreate,
            destroyCallback = ControlEventDefinition_OnDestroy,
            processCallback = InstrumentControlEventDefinition_OnProcess,
          };

      // Note control event definition.
      private static NoteControlEventDefinition _noteControlEventDefinition =
          new NoteControlEventDefinition() {
            createCallback = NoteControlEventDefinition_OnCreate,
            destroyCallback = NoteControlEventDefinition_OnDestroy,
            processCallback = NoteControlEventDefinition_OnProcess,
          };

      // Note off event definition.
      private static NoteOffEventDefinition _noteOffEventDefinition = new NoteOffEventDefinition() {
        createCallback = NoteOffEventDefinition_OnCreate,
        destroyCallback = NoteOffEventDefinition_OnDestroy,
        processCallback = NoteOffEventDefinition_OnProcess,
      };

      // Note on event definition.
      private static NoteOnEventDefinition _noteOnEventDefinition = new NoteOnEventDefinition() {
        createCallback = NoteOnEventDefinition_OnCreate,
        destroyCallback = NoteOnEventDefinition_OnDestroy,
        processCallback = NoteOnEventDefinition_OnProcess,
      };

      // Task definition.
      private static TaskDefinition _taskDefinition = new TaskDefinition() {
        createCallback = TaskDefinition_OnCreate,
        destroyCallback = TaskDefinition_OnDestroy,
        processCallback = TaskDefinition_OnProcess,
      };

      // Map of effects by their handles.
      private static Dictionary<IntPtr, Effect> _effects = null;

      // Map of instruments by their handles.
      private static Dictionary<IntPtr, Instrument> _instruments = null;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Latency in seconds.
      private static double _latency = 0.0;

      // Internal output samples.
      private static double[] _outputSamples = null;

      // Map of scheduled list of task callbacks by their timestamps.
      private static SortedDictionary<double, List<Action>> _scheduledTaskCallbacks = null;

      // Map of performer task callbacks by their handles.
      private static Dictionary<IntPtr, Action> _taskCallbacks = null;

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
          Shutdown();
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
          double lookahead = System.Math.Max(_latency, (double)Time.smoothDeltaTime);
          double nextTimestamp = AudioSettings.dspTime + lookahead;
          while (_scheduledTaskCallbacks.Count > 0) {
            double taskTimestamp = _scheduledTaskCallbacks.ElementAt(0).Key;
            if (taskTimestamp > nextTimestamp) {
              break;
            }
            BarelyMusician_Update(_handle, taskTimestamp);
            var callbacks = _scheduledTaskCallbacks.ElementAt(0).Value;
            for (int i = 0; i < callbacks.Count; ++i) {
              callbacks[i]?.Invoke();
            }
            _scheduledTaskCallbacks.Remove(taskTimestamp);
          }
          BarelyMusician_Update(_handle, nextTimestamp);
        }

        // Initializes the native state.
        private void Initialize() {
          _isShuttingDown = false;
          bool success = BarelyMusician_Create(ref _handle);
          if (!success) {
            Debug.LogError("Failed to initialize BarelyMusician");
            return;
          }
          BarelyMusician_SetTempo(_handle, _tempo);
          var config = AudioSettings.GetConfiguration();
          _latency = (double)(2 * config.dspBufferSize) / (double)config.sampleRate;
          _outputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _effects = new Dictionary<IntPtr, Effect>();
          _instruments = new Dictionary<IntPtr, Instrument>();
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          _taskCallbacks = new Dictionary<IntPtr, Action>();
          BarelyMusician_Update(_handle, AudioSettings.dspTime + _latency);
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
          _effects = null;
          _instruments = null;
          _scheduledTaskCallbacks = null;
          _taskCallbacks = null;
        }
      }

#if !UNITY_EDITOR && UNITY_IOS
      private const string pluginName = "__Internal";
#else
      private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

      [DllImport(pluginName, EntryPoint = "BarelyEffect_Create")]
      private static extern bool BarelyEffect_Create(IntPtr instrument, EffectDefinition definition,
                                                     Int32 processOrder, ref IntPtr outEffect);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_Destroy")]
      private static extern bool BarelyEffect_Destroy(IntPtr effect);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_GetControl")]
      private static extern bool BarelyEffect_GetControl(IntPtr effect, Int32 index,
                                                         ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_GetProcessOrder")]
      private static extern bool BarelyEffect_GetProcessOrder(IntPtr effect,
                                                              ref Int32 outProcessOrder);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_ResetAllControls")]
      private static extern bool BarelyEffect_ResetAllControls(IntPtr effect);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_ResetControl")]
      private static extern bool BarelyEffect_ResetControl(IntPtr effect, Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_SetControl")]
      private static extern bool BarelyEffect_SetControl(IntPtr effect, Int32 index, double value,
                                                         double slopePerBeat);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_SetControlEvent")]
      private static extern bool BarelyEffect_SetControlEvent(IntPtr effect,
                                                              ControlEventDefinition definition,
                                                              IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_SetData")]
      private static extern bool BarelyEffect_SetData(IntPtr effect, IntPtr data, Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyEffect_SetProcessOrder")]
      private static extern bool BarelyEffect_SetProcessOrder(IntPtr effect, Int32 processOrder);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Create")]
      private static extern bool BarelyInstrument_Create(IntPtr musician,
                                                         InstrumentDefinition definition,
                                                         Int32 frameRate, ref IntPtr outInstrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern bool BarelyInstrument_Destroy(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern bool BarelyInstrument_GetControl(IntPtr instrument, Int32 index,
                                                             ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern bool BarelyInstrument_GetNoteControl(IntPtr instrument, double pitch,
                                                                 Int32 index, ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern bool BarelyInstrument_IsNoteOn(IntPtr instrument, double pitch,
                                                           ref bool outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern bool BarelyInstrument_Process(IntPtr instrument,
                                                          [In, Out] double[] outputSamples,
                                                          Int32 outputChannelCount,
                                                          Int32 outputFrameCount, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllControls")]
      private static extern bool BarelyInstrument_ResetAllControls(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllNoteControls")]
      private static extern bool BarelyInstrument_ResetAllNoteControls(IntPtr instrument,
                                                                       double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetControl")]
      private static extern bool BarelyInstrument_ResetControl(IntPtr instrument, Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetNoteControl")]
      private static extern bool BarelyInstrument_ResetNoteControl(IntPtr instrument, double pitch,
                                                                   Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetAllNotesOff")]
      private static extern bool BarelyInstrument_SetAllNotesOff(IntPtr instrument);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern bool BarelyInstrument_SetControl(IntPtr instrument, Int32 index,
                                                             double value, double slopePerBeat);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControlEvent")]
      private static extern bool BarelyInstrument_SetControlEvent(IntPtr instrument,
                                                                  ControlEventDefinition definition,
                                                                  IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
      private static extern bool BarelyInstrument_SetData(IntPtr instrument, IntPtr data,
                                                          Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern bool BarelyInstrument_SetNoteControl(IntPtr instrument, double pitch,
                                                                 Int32 index, double value,
                                                                 double slopePerBeat);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControlEvent")]
      private static extern bool BarelyInstrument_SetNoteControlEvent(
          IntPtr instrument, NoteControlEventDefinition definition, IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern bool BarelyInstrument_SetNoteOff(IntPtr instrument, double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffEvent")]
      private static extern bool BarelyInstrument_SetNoteOffEvent(IntPtr instrument,
                                                                  NoteOffEventDefinition definition,
                                                                  IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern bool BarelyInstrument_SetNoteOn(IntPtr instrument, double pitch,
                                                            double intensity);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnEvent")]
      private static extern bool BarelyInstrument_SetNoteOnEvent(IntPtr instrument,
                                                                 NoteOnEventDefinition definition,
                                                                 IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
      private static extern bool BarelyMusician_Create(ref IntPtr outMusician);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
      private static extern bool BarelyMusician_Destroy(IntPtr musician);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
      private static extern bool BarelyMusician_GetTempo(IntPtr musician, ref double outTempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
      private static extern bool BarelyMusician_GetTimestamp(IntPtr musician,
                                                             ref double outTimestamp);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern bool BarelyMusician_SetTempo(IntPtr musician, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern bool BarelyMusician_Update(IntPtr musician, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Create")]
      private static extern bool BarelyPerformer_Create(IntPtr musician, ref IntPtr outPerformer);

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
                                                   bool isOneOff, double position,
                                                   Int32 processOrder, IntPtr userData,
                                                   ref IntPtr outTask);

      [DllImport(pluginName, EntryPoint = "BarelyTask_Destroy")]
      private static extern bool BarelyTask_Destroy(IntPtr task);

      [DllImport(pluginName, EntryPoint = "BarelyTask_GetPosition")]
      private static extern bool BarelyTask_GetPosition(IntPtr task, ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyTask_GetProcessOrder")]
      private static extern bool BarelyTask_GetProcessOrder(IntPtr task, ref Int32 outProcessOrder);

      [DllImport(pluginName, EntryPoint = "BarelyTask_SetPosition")]
      private static extern bool BarelyTask_SetPosition(IntPtr task, double position);

      [DllImport(pluginName, EntryPoint = "BarelyTask_SetProcessOrder")]
      private static extern bool BarelyTask_SetProcessOrder(IntPtr task, Int32 processOrder);

      [DllImport(pluginName, EntryPoint = "BarelyHighPassEffectDefinition")]
      private static extern EffectDefinition BarelyHighPassEffectDefinition();

      [DllImport(pluginName, EntryPoint = "BarelyLowPassEffectDefinition")]
      private static extern EffectDefinition BarelyLowPassEffectDefinition();

      [DllImport(pluginName, EntryPoint = "BarelyPercussionInstrumentDefinition")]
      private static extern InstrumentDefinition BarelyPercussionInstrumentDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySamplerInstrumentDefinition")]
      private static extern InstrumentDefinition BarelySamplerInstrumentDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySynthInstrumentDefinition")]
      private static extern InstrumentDefinition BarelySynthInstrumentDefinition();
    }
  }
}
