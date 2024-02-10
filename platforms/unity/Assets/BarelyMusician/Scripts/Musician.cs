using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of a musician that governs the tempo for all musical components.
  public static class Musician {
    /// Tempo event callback.
    ///
    /// @param tempo Tempo in beats per minute.
    public delegate void TempoEventCallback(double tempo);
    public static event TempoEventCallback OnTempo;

    [Serializable]
    public class TempoEvent : UnityEngine.Events.UnityEvent<float> {}
    public static TempoEvent OnTempoEvent;

    /// Tempo in beats per minute.
    public static double Tempo {
      get { return _tempo; }
      set {
        if (_tempo != value) {
          Internal.Musician_SetTempo(value);
          _tempo = Internal.Musician_GetTempo();
          OnTempo?.Invoke(_tempo);
          OnTempoEvent?.Invoke((float)_tempo);
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
            if (BarelyArpeggiator_Create(_handle, arpeggiator.ProcessOrder, ref componentHandle)) {
              return;
            }
            break;
          case Repeater repeater:
            if (BarelyRepeater_Create(_handle, repeater.ProcessOrder, ref componentHandle)) {
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

      /// Creates a new effect.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param effect Effect.
      /// @param effectHandle Effect handle.
      public static void Effect_Create(Instrument instrument, Effect effect,
                                       ref IntPtr effectHandle) {
        if (Handle == IntPtr.Zero || effectHandle != IntPtr.Zero) {
          return;
        }
        IntPtr instrumentHandle = Instrument.Internal.GetHandle(instrument);
        if (instrumentHandle == IntPtr.Zero) {
          return;
        }
        EffectDefinition definition;
        switch (effect) {
          case HighPassEffect highPass:
            definition = BarelyHighPassEffect_GetDefinition();
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
        GCHandle controlEventHandle = GCHandle.Alloc(effect);
        BarelyEffect_SetControlEvent(effectHandle, _effectControlEventDefinition,
                                     GCHandle.ToIntPtr(controlEventHandle));
      }

      /// Destroys an effect.
      ///
      /// @param effectHandle Effect handle.
      public static void Effect_Destroy(ref IntPtr effectHandle) {
        if (Handle == IntPtr.Zero || effectHandle == IntPtr.Zero) {
          return;
        }
        if (!BarelyEffect_Destroy(effectHandle)) {
          Debug.LogError("Failed to destroy effect");
        }
        effectHandle = IntPtr.Zero;
      }

      /// Returns the value of an effect control.
      ///
      /// @param effectHandle Effect handle.
      /// @param index Control index.
      /// @return Control value.
      public static double Effect_GetControl(IntPtr effectHandle, int index) {
        double value = 0.0;
        if (!BarelyEffect_GetControl(effectHandle, index, ref value) &&
            effectHandle != IntPtr.Zero) {
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
        if (!BarelyEffect_GetProcessOrder(effectHandle, ref processOrder) &&
            effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get effect process order");
        }
        return processOrder;
      }

      /// Resets all effect control values.
      ///
      /// @param effectHandle Effect handle.
      public static void Effect_ResetAllControls(IntPtr effectHandle) {
        if (!BarelyEffect_ResetAllControls(effectHandle) && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all effect controls");
        }
      }

      /// Resets an effect control value.
      ///
      /// @param effectHandle Effect handle.
      /// @param index Control index.
      public static void Effect_ResetControl(IntPtr effectHandle, int index) {
        if (!BarelyEffect_ResetControl(effectHandle, index) && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset effect control " + index + " value");
        }
      }

      /// Sets an effect control value.
      ///
      /// @param effectHandle Effect handle.
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerSecond Control slope in value change per second.
      public static void Effect_SetControl(IntPtr effectHandle, int index, double value,
                                           double slopePerSecond) {
        if (!BarelyEffect_SetControl(effectHandle, index, value, slopePerSecond) &&
            effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set effect control " + index + " value to " + value +
                         " with slope " + slopePerSecond);
        }
      }

      /// Sets effect data.
      ///
      /// @param effectHandle Effect handle.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Effect_SetData(IntPtr effectHandle, IntPtr dataPtr, int size) {
        if (!BarelyEffect_SetData(effectHandle, dataPtr, size) && effectHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set effect data");
        }
      }

      /// Sets the process order of an effect.
      ///
      /// @param effectHandle Effect handle.
      /// @param processOrder Process order.
      public static void Effect_SetProcessOrder(IntPtr effectHandle, int processOrder) {
        if (!BarelyEffect_SetProcessOrder(effectHandle, processOrder) &&
            effectHandle != IntPtr.Zero) {
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
        GCHandle controlEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetControlEvent(instrumentHandle, _instrumentControlEventDefinition,
                                         GCHandle.ToIntPtr(controlEventHandle));
        GCHandle noteControlEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteControlEvent(instrumentHandle, _noteControlEventDefinition,
                                             GCHandle.ToIntPtr(noteControlEventHandle));
        GCHandle noteOffEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteOffEvent(instrumentHandle, _noteOffEventDefinition,
                                         GCHandle.ToIntPtr(noteOffEventHandle));
        GCHandle noteOnEventHandle = GCHandle.Alloc(instrument);
        BarelyInstrument_SetNoteOnEvent(instrumentHandle, _noteOnEventDefinition,
                                        GCHandle.ToIntPtr(noteOnEventHandle));
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_Destroy(ref IntPtr instrumentHandle) {
        if (Handle == IntPtr.Zero || instrumentHandle == IntPtr.Zero) {
          return;
        }
        if (!BarelyInstrument_Destroy(instrumentHandle)) {
          Debug.LogError("Failed to destroy instrument");
        }
        instrumentHandle = IntPtr.Zero;
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param index Control index.
      /// @return Control value.
      public static double Instrument_GetControl(IntPtr instrumentHandle, int index) {
        double value = 0.0;
        if (!BarelyInstrument_GetControl(instrumentHandle, index, ref value) &&
            instrumentHandle != IntPtr.Zero) {
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
        if (!BarelyInstrument_GetNoteControl(instrumentHandle, pitch, index, ref value) &&
            instrumentHandle != IntPtr.Zero) {
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
        if (!BarelyInstrument_IsNoteOn(instrumentHandle, pitch, ref isNoteOn) &&
            instrumentHandle != IntPtr.Zero) {
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
        if (BarelyInstrument_Process(instrumentHandle, OutputSamples, outputChannelCount,
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

      /// Resets all instrument control values.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_ResetAllControls(IntPtr instrumentHandle) {
        if (!BarelyInstrument_ResetAllControls(instrumentHandle) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument controls");
        }
      }

      /// Resets all instrument note control values.
      ///
      /// @param instrumentHandle Instrument handle.
      public static void Instrument_ResetAllNoteControls(IntPtr instrumentHandle, double pitch) {
        if (!BarelyInstrument_ResetAllNoteControls(instrumentHandle, pitch) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument note pitch " + pitch + " controls");
        }
      }

      /// Resets an instrument control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param index Control index.
      public static void Instrument_ResetControl(IntPtr instrumentHandle, int index) {
        if (!BarelyInstrument_ResetControl(instrumentHandle, index) &&
            instrumentHandle != IntPtr.Zero) {
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
        if (!BarelyInstrument_ResetNoteControl(instrumentHandle, pitch, index) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument note pitch " + pitch + " control " + index +
                         " value");
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
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerSecond Control slope in value change per second.
      public static void Instrument_SetControl(IntPtr instrumentHandle, int index, double value,
                                               double slopePerSecond) {
        if (!BarelyInstrument_SetControl(instrumentHandle, index, value, slopePerSecond) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument control " + index + " value to " + value +
                         " with slope " + slopePerSecond);
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Instrument_SetData(IntPtr instrumentHandle, IntPtr dataPtr, int size) {
        if (!BarelyInstrument_SetData(instrumentHandle, dataPtr, size) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument data");
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @param value Note control value.
      /// @param slopePerSecond Note control slope in value change per second.
      public static void Instrument_SetNoteControl(IntPtr instrumentHandle, double pitch, int index,
                                                   double value, double slopePerSecond) {
        if (!BarelyInstrument_SetNoteControl(instrumentHandle, pitch, index, value,
                                             slopePerSecond) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument note pitch " + pitch + " control " + index +
                         " value to " + value + " with slope " + slopePerSecond);
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOff(IntPtr instrumentHandle, double pitch) {
        if (!BarelyInstrument_SetNoteOff(instrumentHandle, pitch) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + "");
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrumentHandle Instrument handle.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOn(IntPtr instrumentHandle, double pitch,
                                              double intensity) {
        if (!BarelyInstrument_SetNoteOn(instrumentHandle, pitch, intensity) &&
            instrumentHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity");
        }
      }

      /// Returns the corresponding number of seconds for a given number of musician beats.
      ///
      /// @param frames Number of beats.
      /// @return Number of seconds.
      public static double Musician_GetSecondsFromBeats(double beats) {
        double seconds = 0.0;
        if (!BarelyMusician_GetSecondsFromBeats(Handle, beats, ref seconds) &&
            _handle != IntPtr.Zero) {
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
        if (!BarelyMusician_GetBeatsFromSeconds(Handle, seconds, ref beats) &&
            _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician beats for " + seconds + " seconds");
        }
        return beats;
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        double tempo = 0.0;
        if (!BarelyMusician_GetTempo(Handle, ref tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo");
        }
        return tempo;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        double timestamp = 0.0;
        if (!BarelyMusician_GetTimestamp(Handle, ref timestamp) && _handle != IntPtr.Zero) {
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
        if (!BarelyMusician_SetTempo(Handle, tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo");
        }
      }

      /// Cancels all one-off tasks.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_CancelAllOneOffTasks(IntPtr performerHandle) {
        if (!BarelyPerformer_CancelAllOneOffTasks(performerHandle) &&
            performerHandle != IntPtr.Zero) {
          Debug.LogError("Failed to cancel all one-off tasks");
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
      }

      /// Destroys a performer.
      ///
      /// @param performerHandle Performer handle.
      public static void Performer_Destroy(ref IntPtr performerHandle) {
        if (Handle == IntPtr.Zero || performerHandle == IntPtr.Zero) {
          return;
        }
        if (!BarelyPerformer_Destroy(performerHandle)) {
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

      /// Schedules a one-off task.
      ///
      /// @param performerHandle Performer handle.
      /// @param callback Task callback.
      /// @param position Task position.
      /// @param processOrder Task process order.
      public static void Performer_ScheduleOneOffTask(IntPtr performerHandle, Action callback,
                                                      double position, int processOrder) {
        if (Handle == null || callback == null) {
          return;
        }
        GCHandle handle = GCHandle.Alloc(callback);
        if (!BarelyPerformer_ScheduleOneOffTask(performerHandle, _taskDefinition, position,
                                                processOrder, GCHandle.ToIntPtr(handle)) &&
            performerHandle != IntPtr.Zero) {
          handle.Free();
          Debug.LogError("Failed to set performer loop begin position");
        }
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

      /// Creates a new task.
      ///
      /// @param performerHandle Performer handle.
      /// @param callback Task callback.
      /// @param position Task position.
      /// @param processOrder Task process order.
      /// @param taskHandle Task handle.
      public static void Task_Create(IntPtr performerHandle, Action callback, double position,
                                     int processOrder, ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle != IntPtr.Zero) {
          return;
        }
        GCHandle handle = GCHandle.Alloc(callback);
        if (!BarelyTask_Create(performerHandle, _taskDefinition, position, processOrder,
                               GCHandle.ToIntPtr(handle), ref taskHandle)) {
          handle.Free();
        }
      }

      /// Destroys a task.
      ///
      /// @param taskHandle Task handle.
      public static void Task_Destroy(ref IntPtr taskHandle) {
        if (Handle == IntPtr.Zero || taskHandle == IntPtr.Zero) {
          return;
        }
        if (!BarelyTask_Destroy(taskHandle)) {
          Debug.LogError("Failed to destroy performer task");
        }
        taskHandle = IntPtr.Zero;
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

      /// Returns the process order of a task.
      ///
      /// @param taskHandle Task handle.
      /// @return Process order.
      public static int Task_GetProcessOrder(IntPtr taskHandle) {
        Int32 processOrder = 0;
        if (!BarelyTask_GetProcessOrder(taskHandle, ref processOrder) &&
            taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task process order");
        }
        return processOrder;
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

      /// Sets the process order of a task.
      ///
      /// @param taskHandle Task handle.
      /// @param processOrder Process order.
      public static void Task_SetProcessOrder(IntPtr taskHandle, int processOrder) {
        if (!BarelyTask_SetProcessOrder(taskHandle, processOrder) && taskHandle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task process order");
        }
      }

      /// Returns whether an arpeggiator note is on or not.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Arpeggiator_IsNoteOn(IntPtr arpeggiatorHandle, double pitch) {
        bool isNoteOn = false;
        if (!BarelyArpeggiator_IsNoteOn(arpeggiatorHandle, pitch, ref isNoteOn) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to get if arpeggiator note pitch " + pitch + " is on");
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
      public static void Arpeggiator_SetGateRatio(IntPtr arpeggiatorHandle, double gateRatio) {
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
      public static void Arpeggiator_SetNoteOff(IntPtr arpeggiatorHandle, double pitch) {
        if (!BarelyArpeggiator_SetNoteOff(arpeggiatorHandle, pitch) &&
            arpeggiatorHandle != IntPtr.Zero) {
          Debug.LogError("Failed to stop arpeggiator note " + pitch);
        }
      }

      /// Sets an arpeggiator note on.
      ///
      /// @param arpeggiatorHandle Arpeggiator handle.
      /// @param pitch Note pitch.
      public static void Arpeggiator_SetNoteOn(IntPtr arpeggiatorHandle, double pitch) {
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
      /// @param pitchOr Note pitch or silence.
      /// @param length Note length in beats.
      public static void Repeater_Push(IntPtr repeaterHandle, double? pitchOr, int length) {
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
      public static void Repeater_Start(IntPtr repeaterHandle, double pitchOffset) {
        if (!BarelyRepeater_Start(repeaterHandle, pitchOffset) && repeaterHandle != IntPtr.Zero) {
          Debug.LogError("Failed to start repeater with pitch offset " + pitchOffset);
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

      // Internal output samples.
      public static double[] OutputSamples { get; private set; } = null;

      // Control event definition create callback.
      private delegate void ControlEventDefinition_CreateCallback(ref IntPtr state,
                                                                  IntPtr userData);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_CreateCallback))]
      private static void ControlEventDefinition_OnCreate(ref IntPtr state, IntPtr userData) {
        state = userData;
      }

      // Control event definition destroy callback.
      private delegate void ControlEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_DestroyCallback))]
      private static void ControlEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Control event definition process callback.
      private delegate void ControlEventDefinition_ProcessCallback(ref IntPtr state, int index,
                                                                   double value);
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_ProcessCallback))]
      private static void EffectControlEventDefinition_OnProcess(ref IntPtr state, int index,
                                                                 double value) {
        Effect effect = GCHandle.FromIntPtr(state).Target as Effect;
        Effect.Internal.OnControlEvent(effect, index, value);
      }
      [AOT.MonoPInvokeCallback(typeof(ControlEventDefinition_ProcessCallback))]
      private static void InstrumentControlEventDefinition_OnProcess(ref IntPtr state, int index,
                                                                     double value) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnControlEvent(instrument, index, value);
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
        state = userData;
      }

      // Note control event definition destroy callback.
      private delegate void NoteControlEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_DestroyCallback))]
      private static void NoteControlEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Note control event definition process callback.
      private delegate void NoteControlEventDefinition_ProcessCallback(ref IntPtr state,
                                                                       double pitch, int index,
                                                                       double value);
      [AOT.MonoPInvokeCallback(typeof(NoteControlEventDefinition_ProcessCallback))]
      private static void NoteControlEventDefinition_OnProcess(ref IntPtr state, double pitch,
                                                               int index, double value) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnNoteControlEvent(instrument, pitch, index, value);
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
        state = userData;
      }

      // Note off event definition destroy callback.
      private delegate void NoteOffEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_DestroyCallback))]
      private static void NoteOffEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Note off event definition process callback.
      private delegate void NoteOffEventDefinition_ProcessCallback(ref IntPtr state, double pitch);
      [AOT.MonoPInvokeCallback(typeof(NoteOffEventDefinition_ProcessCallback))]
      private static void NoteOffEventDefinition_OnProcess(ref IntPtr state, double pitch) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnNoteOffEvent(instrument, pitch);
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
        state = userData;
      }

      // Note on event definition destroy callback.
      private delegate void NoteOnEventDefinition_DestroyCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_DestroyCallback))]
      private static void NoteOnEventDefinition_OnDestroy(ref IntPtr state) {
        GCHandle.FromIntPtr(state).Free();
      }

      // Note on event definition process callback.
      private delegate void NoteOnEventDefinition_ProcessCallback(ref IntPtr state, double pitch,
                                                                  double intensity);
      [AOT.MonoPInvokeCallback(typeof(NoteOnEventDefinition_ProcessCallback))]
      private static void NoteOnEventDefinition_OnProcess(ref IntPtr state, double pitch,
                                                          double intensity) {
        Instrument instrument = GCHandle.FromIntPtr(state).Target as Instrument;
        Instrument.Internal.OnNoteOnEvent(instrument, pitch, intensity);
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
              _isShuttingDown = true;
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
          if (!BarelyMusician_Create(ref _handle)) {
            Debug.LogError("Failed to initialize BarelyMusician");
            return;
          }
          BarelyMusician_SetTempo(_handle, _tempo);
          var config = AudioSettings.GetConfiguration();
          OutputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _latency = (double)config.dspBufferSize / config.sampleRate;
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          BarelyMusician_Update(_handle, GetNextTimestamp());
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
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
                                                         double slopePerSecond);

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
                                                             double value, double slopePerSecond);

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
                                                                 double slopePerSecond);

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

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CancelAllOneOffTasks")]
      private static extern bool BarelyPerformer_CancelAllOneOffTasks(IntPtr performer);

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

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_ScheduleOneOffTask")]
      private static extern bool BarelyPerformer_ScheduleOneOffTask(IntPtr performer,
                                                                    TaskDefinition definition,
                                                                    double position,
                                                                    Int32 processOrder,
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
                                                   double position, Int32 processOrder,
                                                   IntPtr userData, ref IntPtr outTask);

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
