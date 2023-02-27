using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Musician.
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

    /// Schedules a task at a specific time.
    ///
    /// @param callback Task process callback.
    /// @param dspTime Time in seconds.
    public static void ScheduleTask(Action callback, double dspTime) {
      Internal.Musician_ScheduleTask(callback, dspTime);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Invalid identifier.
      public const Int64 InvalidId = 0;

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

      /// Custom instrument interface.
      public interface CustomInstrumentInterface {
        /// Returns the instrument definition.
        ///
        /// @return Instrument definition.
        public InstrumentDefinition GetDefinition();
      }

      /// Creates a new instrument.
      ///
      /// @param instrument Instrument.
      /// @param instrumentId Instrument identifier.
      public static void Instrument_Create(Instrument instrument, ref Int64 instrumentId) {
        if (instrumentId != InvalidId) {
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
        Status status = BarelyInstrument_Create(Handle, definition, AudioSettings.outputSampleRate,
                                                ref instrumentId);
        if (instrument.GetType().IsSubclassOf(typeof(CustomInstrumentInterface))) {
          if (definition.controlDefinitionCount > 0) {
            Marshal.FreeHGlobal(definition.controlDefinitions);
          }
          if (definition.noteControlDefinitionCount > 0) {
            Marshal.FreeHGlobal(definition.noteControlDefinitions);
          }
        }
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to create instrument " + instrument.name + ": " + status);
          return;
        }
        _instruments?.Add(instrumentId, instrument);
        BarelyInstrument_SetControlEventCallback(_handle, instrumentId, Instrument_OnControlEvent,
                                                 ref instrumentId);
        BarelyInstrument_SetNoteControlEventCallback(
            _handle, instrumentId, Instrument_OnNoteControlEvent, ref instrumentId);
        BarelyInstrument_SetNoteOffEventCallback(_handle, instrumentId, Instrument_OnNoteOffEvent,
                                                 ref instrumentId);
        BarelyInstrument_SetNoteOnEventCallback(_handle, instrumentId, Instrument_OnNoteOnEvent,
                                                ref instrumentId);
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentId Instrument identifier.
      public static void Instrument_Destroy(ref Int64 instrumentId) {
        if (instrumentId == InvalidId) {
          return;
        }
        Status status = BarelyInstrument_Destroy(Handle, instrumentId);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy instrument " + instrumentId + ": " + status);
          return;
        }
        _instruments?.Remove(instrumentId);
        instrumentId = InvalidId;
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param index Control index.
      /// @return Control value.
      public static double Instrument_GetControl(Int64 instrumentId, int index) {
        double value = 0.0;
        Status status = BarelyInstrument_GetControl(Handle, instrumentId, index, ref value);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to get instrument control " + index + " value for " +
                         instrumentId + ": " + status);
        }
        return value;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @return Note control value.
      public static double Instrument_GetNoteControl(Int64 instrumentId, double pitch, int index) {
        double value = 0.0;
        Status status =
            BarelyInstrument_GetNoteControl(Handle, instrumentId, pitch, index, ref value);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to get instrument note pitch " + pitch + " control " + index +
                         " value for " + instrumentId + ": " + status);
        }
        return value;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(Int64 instrumentId, double pitch) {
        bool isNoteOn = false;
        Status status = BarelyInstrument_IsNoteOn(Handle, instrumentId, pitch, ref isNoteOn);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to get if instrument note pitch " + pitch + " is on for " +
                         instrumentId + ": " + status);
        }
        return isNoteOn;
      }

      /// Processes instrument output samples.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Instrument_Process(Int64 instrumentId, float[] outputSamples,
                                            int outputChannelCount) {
        Status status = BarelyInstrument_Process(
            Handle, instrumentId, _outputSamples, outputChannelCount,
            outputSamples.Length / outputChannelCount, AudioSettings.dspTime);
        if (IsOk(status)) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = (float)_outputSamples[i];
          }
        } else {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
        }
      }

      /// Resets all instrument control values.
      ///
      /// @param instrumentId Instrument identifier.
      public static void Instrument_ResetAllControls(Int64 instrumentId) {
        Status status = BarelyInstrument_ResetAllControls(Handle, instrumentId);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to reset all instrument controls for " + instrumentId + ": " +
                         status);
        }
      }

      /// Resets all instrument note control values.
      ///
      /// @param instrumentId Instrument identifier.
      public static void Instrument_ResetAllNoteControls(Int64 instrumentId, double pitch) {
        Status status = BarelyInstrument_ResetAllNoteControls(Handle, instrumentId, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to reset all instrument note pitch " + pitch + " controls for " +
                         instrumentId + ": " + status);
        }
      }

      /// Resets an instrument control value.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param index Control index.
      public static void Instrument_ResetControl(Int64 instrumentId, int index) {
        Status status = BarelyInstrument_ResetControl(Handle, instrumentId, index);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to reset instrument control " + index + " for " + instrumentId +
                         ": " + status);
        }
      }

      /// Resets an instrument note control value.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      public static void Instrument_ResetNoteControl(Int64 instrumentId, double pitch, int index) {
        Status status = BarelyInstrument_ResetNoteControl(Handle, instrumentId, pitch, index);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to reset instrument note pitch " + pitch + " control " + index +
                         " for " + instrumentId + ": " + status);
        }
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrumentId Instrument identifier.
      /// @return True if success, false otherwise.
      public static void Instrument_SetAllNotesOff(Int64 instrumentId) {
        Status status = BarelyInstrument_SetAllNotesOff(Handle, instrumentId);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to stop all instrument notes for " + instrumentId + ": " + status);
        }
      }

      /// Sets an instrument control value.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerBeat Control slope in value change per beat.
      public static void Instrument_SetControl(Int64 instrumentId, int index, double value,
                                               double slopePerBeat) {
        Status status =
            BarelyInstrument_SetControl(Handle, instrumentId, index, value, slopePerBeat);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to set instrument control " + index + " value to " + value +
                         " with slope " + slopePerBeat + " for " + instrumentId + ": " + status);
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param dataPtr Pointer to data.
      /// @param size Data size in bytes.
      public static void Instrument_SetData(Int64 instrumentId, IntPtr dataPtr, int size) {
        Status status = BarelyInstrument_SetData(Handle, instrumentId, dataPtr, size);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to set instrument data to " + dataPtr + " for " + instrumentId +
                         ": " + status);
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @param value Note control value.
      /// @param slopePerBeat Note control slope in value change per beat.
      public static void Instrument_SetNoteControl(Int64 instrumentId, double pitch, int index,
                                                   double value, double slopePerBeat) {
        Status status = BarelyInstrument_SetNoteControl(Handle, instrumentId, pitch, index, value,
                                                        slopePerBeat);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to set instrument note pitch " + pitch + " control " + index +
                         " value to " + value + " with slope " + slopePerBeat + " for " +
                         instrumentId + ": " + status);
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @return True if success, false otherwise.
      public static void Instrument_SetNoteOff(Int64 instrumentId, double pitch) {
        Status status = BarelyInstrument_SetNoteOff(Handle, instrumentId, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to stop instrument note " + pitch + " for " + instrumentId + ": " +
                         status);
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOn(Int64 instrumentId, double pitch, double intensity) {
        Status status = BarelyInstrument_SetNoteOn(Handle, instrumentId, pitch, intensity);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity for " + instrumentId + ": " + status);
        }
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        double tempo = 0.0;
        Status status = BarelyMusician_GetTempo(Handle, ref tempo);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo: " + status);
        }
        return tempo;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        double timestamp = 0.0;
        Status status = BarelyMusician_GetTimestamp(Handle, ref timestamp);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician timestamp: " + status);
        }
        return timestamp;
      }

      /// Schedules a new musician task.
      ///
      /// @param callback Task process callback.
      /// @param timestamp Task timestamp in seconds.
      public static void Musician_ScheduleTask(Action callback, double timestamp) {
        if (timestamp < Timestamp) {
          Debug.LogError("Failed to create musician task at " + timestamp + ": " +
                         Status.INVALID_ARGUMENT);
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
        Status status = BarelyMusician_SetTempo(Handle, tempo);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo: " + status);
        }
      }

      /// Creates a new performer.
      ///
      /// @param performer Performer.
      /// @param performerId Performer identifier.
      public static void Performer_Create(Performer performer, ref Int64 performerId) {
        if (performerId != InvalidId) {
          return;
        }
        Status status = BarelyPerformer_Create(Handle, ref performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to create performer " + performer.name + ": " + status);
        }
      }

      /// Creates a new performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param callback Task callback.
      /// @param isOneOff True if one off task, false otherwise.
      /// @param position Task position.
      /// @param processOrder Task process order.
      /// @param taskId Task identifier.
      public static void Performer_CreateTask(Int64 performerId, Action callback, bool isOneOff,
                                              double position, int processOrder, ref Int64 taskId) {
        if (taskId != InvalidId) {
          return;
        }
        IntPtr taskIdPtr = Marshal.AllocHGlobal(Marshal.SizeOf<Int64>());
        Status status = BarelyPerformer_CreateTask(Handle, performerId, _taskDefinition, isOneOff,
                                                   position, processOrder, taskIdPtr, ref taskId);
        if (!IsOk(status)) {
          Marshal.DestroyStructure<Int64>(taskIdPtr);
          if (_handle != IntPtr.Zero && performerId != InvalidId) {
            Debug.LogError("Failed to create performer " + performerId + " task': " + status);
            return;
          }
        }
        Marshal.WriteInt64(taskIdPtr, taskId);
        _taskCallbacks.Add(taskId, callback);
      }

      /// Destroys a performer.
      ///
      /// @param performerId Performer identifier.
      public static void Performer_Destroy(ref Int64 performerId) {
        if (performerId == InvalidId) {
          return;
        }
        Status status = BarelyPerformer_Destroy(Handle, performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to destroy performer " + performerId + ": " + status);
        }
        performerId = InvalidId;
      }

      /// Destroys a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      public static void Performer_DestroyTask(Int64 performerId, ref Int64 taskId) {
        if (taskId == InvalidId) {
          return;
        }
        Status status = BarelyPerformer_DestroyTask(Handle, performerId, taskId);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId &&
            taskId != InvalidId) {
          Debug.LogError("Failed to destroy performer task " + taskId + ": " + status);
        }
        taskId = InvalidId;
        _taskCallbacks.Remove(taskId);
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(Int64 performerId) {
        double loopBeginPosition = 0.0;
        Status status =
            BarelyPerformer_GetLoopBeginPosition(Handle, performerId, ref loopBeginPosition);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer loop begin position: " + status);
        }
        return loopBeginPosition;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(Int64 performerId) {
        double loopLength = 0.0;
        Status status = BarelyPerformer_GetLoopLength(Handle, performerId, ref loopLength);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer loop length: " + status);
        }
        return loopLength;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @return Position in beats.
      public static double Performer_GetPosition(Int64 performerId) {
        double position = 0.0;
        Status status = BarelyPerformer_GetPosition(Handle, performerId, ref position);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer position: " + status);
        }
        return position;
      }

      /// Returns the position of a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      /// @return Position in beats.
      public static double Performer_GetTaskPosition(Int64 performerId, Int64 taskId) {
        double position = 0.0;
        Status status = BarelyPerformer_GetTaskPosition(Handle, performerId, taskId, ref position);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId &&
            taskId != InvalidId) {
          Debug.LogError("Failed to get performer task position: " + status);
        }
        return position;
      }

      /// Returns the process order of a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      /// @return Process order.
      public static int Performer_GetTaskProcessOrder(Int64 performerId, Int64 taskId) {
        Int32 processOrder = 0;
        Status status =
            BarelyPerformer_GetTaskProcessOrder(Handle, performerId, taskId, ref processOrder);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId &&
            taskId != InvalidId) {
          Debug.LogError("Failed to get performer task process order: " + status);
        }
        return processOrder;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerId Performer identifier.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(Int64 performerId) {
        bool isLooping = false;
        Status status = BarelyPerformer_IsLooping(Handle, performerId, ref isLooping);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer looping: " + status);
        }
        return isLooping;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerId Performer identifier.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(Int64 performerId) {
        bool isPlaying = false;
        Status status = BarelyPerformer_IsPlaying(Handle, performerId, ref isPlaying);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer playing: " + status);
        }
        return isPlaying;
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(Int64 performerId,
                                                        double loopBeginPosition) {
        Status status =
            BarelyPerformer_SetLoopBeginPosition(Handle, performerId, loopBeginPosition);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to set performer loop begin position: " + status);
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(Int64 performerId, double loopLength) {
        Status status = BarelyPerformer_SetLoopLength(Handle, performerId, loopLength);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to set performer loop length: " + status);
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerId Performer identifier.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(Int64 performerId, bool isLooping) {
        Status status = BarelyPerformer_SetLooping(Handle, performerId, isLooping);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to set performer looping: " + status);
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @param position Position in beats.
      public static void Performer_SetPosition(Int64 performerId, double position) {
        Status status = BarelyPerformer_SetPosition(Handle, performerId, position);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to set performer position: " + status);
        }
      }

      /// Sets the position of a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      /// @param position Position in beats.
      public static void Performer_SetTaskPosition(Int64 performerId, Int64 taskId,
                                                   double position) {
        Status status = BarelyPerformer_SetTaskPosition(Handle, performerId, taskId, position);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId &&
            taskId != InvalidId) {
          Debug.LogError("Failed to set performer task position: " + status);
        }
      }

      /// Sets the process order of a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      /// @param processOrder Process order.
      public static void Performer_SetTaskProcessOrder(Int64 performerId, Int64 taskId,
                                                       int processOrder) {
        Status status =
            BarelyPerformer_SetTaskProcessOrder(Handle, performerId, taskId, processOrder);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId &&
            taskId != InvalidId) {
          Debug.LogError("Failed to set performer task process order: " + status);
        }
      }

      /// Starts a performer.
      ///
      /// @param performerId Performer identifier.
      public static void Performer_Start(Int64 performerId) {
        Status status = BarelyPerformer_Start(Handle, performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to start performer: " + status);
        }
      }

      /// Stops a performer.
      ///
      /// @param performerId Performer identifier.
      public static void Performer_Stop(Int64 performerId) {
        Status status = BarelyPerformer_Stop(Handle, performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to stop performer: " + status);
        }
      }

      // Status enum.
      private enum Status {
        // Success.
        OK = 0,
        // Invalid argument error.
        INVALID_ARGUMENT = 1,
        // Not found error.
        NOT_FOUND = 2,
        // Unimplemented error.
        UNIMPLEMENTED = 3,
        // Internal error.
        INTERNAL = 4,
      }

      // Returns whether a status is okay or not.
      private static bool IsOk(Status status) {
        return (status == Status.OK);
      }

      // Instrument control event callback.
      private delegate void Instrument_ControlEventCallback(int index, double value,
                                                            ref Int64 userData);
      [AOT.MonoPInvokeCallback(typeof(Instrument_ControlEventCallback))]
      private static void Instrument_OnControlEvent(int index, double value, ref Int64 userData) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(userData, out instrument)) {
          Instrument.Internal.OnControlEvent(instrument, index, value);
        }
      }

      // Instrument note control event callback.
      private delegate void Instrument_NoteControlEventCallback(double pitch, int index,
                                                                double value, ref Int64 userData);
      [AOT.MonoPInvokeCallback(typeof(Instrument_NoteControlEventCallback))]
      private static void Instrument_OnNoteControlEvent(double pitch, int index, double value,
                                                        ref Int64 userData) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(userData, out instrument)) {
          Instrument.Internal.OnNoteControlEvent(instrument, pitch, index, value);
        }
      }

      // Instrument note off event callback.
      private delegate void Instrument_NoteOffEventCallback(double pitch, ref Int64 userData);
      [AOT.MonoPInvokeCallback(typeof(Instrument_NoteOffEventCallback))]
      private static void Instrument_OnNoteOffEvent(double pitch, ref Int64 userData) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(userData, out instrument)) {
          Instrument.Internal.OnNoteOffEvent(instrument, pitch);
        }
      }

      // Instrument note on event callback.
      private delegate void Instrument_NoteOnEventCallback(double pitch, double intensity,
                                                           ref Int64 userData);
      [AOT.MonoPInvokeCallback(typeof(Instrument_NoteOnEventCallback))]
      private static void Instrument_OnNoteOnEvent(double pitch, double intensity,
                                                   ref Int64 userData) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(userData, out instrument)) {
          Instrument.Internal.OnNoteOnEvent(instrument, pitch, intensity);
        }
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
          _taskCallbacks?.Remove(Marshal.PtrToStructure<Int64>(state));
          Marshal.DestroyStructure<Int64>(state);
        }
      }

      // Task definition process callback.
      private delegate void TaskDefinition_ProcessCallback(ref IntPtr state);
      [AOT.MonoPInvokeCallback(typeof(TaskDefinition_ProcessCallback))]
      private static void TaskDefinition_OnProcess(ref IntPtr state) {
        Action callback = null;
        if (_taskCallbacks.TryGetValue(Marshal.PtrToStructure<Int64>(state), out callback)) {
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

      // Map of instruments by their identifiers.
      private static Dictionary<Int64, Instrument> _instruments = null;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Latency in seconds.
      private static double _latency = 0.0;

      // Internal output samples.
      private static double[] _outputSamples = null;

      // Map of scheduled list of task callbacks by their timestamps.
      private static SortedDictionary<double, List<Action>> _scheduledTaskCallbacks = null;

      // Map of performer task callbacks by their identifiers.
      private static Dictionary<Int64, Action> _taskCallbacks = null;

      // Task definition.
      private static TaskDefinition _taskDefinition = new TaskDefinition() {
        createCallback = TaskDefinition_OnCreate,
        destroyCallback = TaskDefinition_OnDestroy,
        processCallback = TaskDefinition_OnProcess,
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
          Status status = BarelyMusician_Create(ref _handle);
          if (!IsOk(status)) {
            Debug.LogError("Failed to initialize BarelyMusician: " + status);
            return;
          }
          BarelyMusician_SetTempo(_handle, _tempo);
          var config = AudioSettings.GetConfiguration();
          _latency = (double)(config.dspBufferSize) / (double)config.sampleRate;
          _outputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _instruments = new Dictionary<Int64, Instrument>();
          _scheduledTaskCallbacks = new SortedDictionary<double, List<Action>>();
          _taskCallbacks = new Dictionary<Int64, Action>();
          BarelyMusician_Update(_handle, AudioSettings.dspTime + _latency);
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
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

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Create")]
      private static extern Status BarelyInstrument_Create(IntPtr handle,
                                                           InstrumentDefinition definition,
                                                           Int32 frameRate,
                                                           ref Int64 outInstrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern Status BarelyInstrument_Destroy(IntPtr handle, Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern Status BarelyInstrument_GetControl(IntPtr handle, Int64 instrumentId,
                                                               Int32 index, ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern Status BarelyInstrument_GetNoteControl(IntPtr handle,
                                                                   Int64 instrumentId, double pitch,
                                                                   Int32 index,
                                                                   ref double outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern Status BarelyInstrument_IsNoteOn(IntPtr handle, Int64 instrumentId,
                                                             double pitch, ref bool outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern Status BarelyInstrument_Process(IntPtr handle, Int64 instrumentId,
                                                            [In, Out] double[] outputSamples,
                                                            Int32 outputChannelCount,
                                                            Int32 outputFrameCount,
                                                            double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllControls")]
      private static extern Status BarelyInstrument_ResetAllControls(IntPtr handle,
                                                                     Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllNoteControls")]
      private static extern Status BarelyInstrument_ResetAllNoteControls(IntPtr handle,
                                                                         Int64 instrumentId,
                                                                         double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetControl")]
      private static extern Status BarelyInstrument_ResetControl(IntPtr handle, Int64 instrumentId,
                                                                 Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetNoteControl")]
      private static extern Status BarelyInstrument_ResetNoteControl(IntPtr handle,
                                                                     Int64 instrumentId,
                                                                     double pitch, Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetAllNotesOff")]
      private static extern Status BarelyInstrument_SetAllNotesOff(IntPtr handle,
                                                                   Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern Status BarelyInstrument_SetControl(IntPtr handle, Int64 instrumentId,
                                                               Int32 index, double value,
                                                               double slopePerBeat);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetControlEventCallback")]
      private static extern Status BarelyInstrument_SetControlEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument_ControlEventCallback callback,
          ref Int64 userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
      private static extern Status BarelyInstrument_SetData(IntPtr handle, Int64 instrumentId,
                                                            IntPtr data, Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern Status BarelyInstrument_SetNoteControl(IntPtr handle,
                                                                   Int64 instrumentId, double pitch,
                                                                   Int32 index, double value,
                                                                   double slopePerBeat);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControlEventCallback")]
      private static extern Status BarelyInstrument_SetNoteControlEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument_NoteControlEventCallback callback,
          ref Int64 userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern Status BarelyInstrument_SetNoteOff(IntPtr handle, Int64 instrumentId,
                                                               double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffEventCallback")]
      private static extern Status BarelyInstrument_SetNoteOffEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument_NoteOffEventCallback callback,
          ref Int64 userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern Status BarelyInstrument_SetNoteOn(IntPtr handle, Int64 instrumentId,
                                                              double pitch, double intensity);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnEventCallback")]
      private static extern Status BarelyInstrument_SetNoteOnEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument_NoteOnEventCallback callback,
          ref Int64 userData);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
      private static extern Status BarelyMusician_Create(ref IntPtr outHandle);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
      private static extern Status BarelyMusician_Destroy(IntPtr handle);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
      private static extern Status BarelyMusician_GetTempo(IntPtr handle, ref double outTempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
      private static extern Status BarelyMusician_GetTimestamp(IntPtr handle,
                                                               ref double outTimestamp);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern Status BarelyMusician_SetTempo(IntPtr handle, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern Status BarelyMusician_Update(IntPtr handle, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Create")]
      private static extern Status BarelyPerformer_Create(IntPtr handle, ref Int64 outPerformerId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CreateTask")]
      private static extern Status BarelyPerformer_CreateTask(IntPtr handle, Int64 performerId,
                                                              TaskDefinition definition,
                                                              bool isOneOff, double position,
                                                              Int32 processOrder, IntPtr userData,
                                                              ref Int64 outTaskId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Destroy")]
      private static extern Status BarelyPerformer_Destroy(IntPtr handle, Int64 performerId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_DestroyTask")]
      private static extern Status BarelyPerformer_DestroyTask(IntPtr handle, Int64 performerId,
                                                               Int64 taskId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopBeginPosition")]
      private static extern Status BarelyPerformer_GetLoopBeginPosition(
          IntPtr handle, Int64 performerId, ref double outLoopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopLength")]
      private static extern Status BarelyPerformer_GetLoopLength(IntPtr handle, Int64 performerId,
                                                                 ref double outLoopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern Status BarelyPerformer_GetPosition(IntPtr handle, Int64 performerId,
                                                               ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetTaskPosition")]
      private static extern Status BarelyPerformer_GetTaskPosition(IntPtr handle, Int64 performerId,
                                                                   Int64 taskId,
                                                                   ref double outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetTaskProcessOrder")]
      private static extern Status BarelyPerformer_GetTaskProcessOrder(IntPtr handle,
                                                                       Int64 performerId,
                                                                       Int64 taskId,
                                                                       ref Int32 outProcessOrder);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsLooping")]
      private static extern Status BarelyPerformer_IsLooping(IntPtr handle, Int64 performerId,
                                                             ref bool outIsLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsPlaying")]
      private static extern Status BarelyPerformer_IsPlaying(IntPtr handle, Int64 performerId,
                                                             ref bool outIsPlaying);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLoopBeginPosition")]
      private static extern Status BarelyPerformer_SetLoopBeginPosition(IntPtr handle,
                                                                        Int64 performerId,
                                                                        double loopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLoopLength")]
      private static extern Status BarelyPerformer_SetLoopLength(IntPtr handle, Int64 performerId,
                                                                 double loopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetLooping")]
      private static extern Status BarelyPerformer_SetLooping(IntPtr handle, Int64 performerId,
                                                              bool isLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetPosition")]
      private static extern Status BarelyPerformer_SetPosition(IntPtr handle, Int64 performerId,
                                                               double position);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetTaskPosition")]
      private static extern Status BarelyPerformer_SetTaskPosition(IntPtr handle, Int64 performerId,
                                                                   Int64 taskId, double position);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_SetTaskProcessOrder")]
      private static extern Status BarelyPerformer_SetTaskProcessOrder(IntPtr handle,
                                                                       Int64 performerId,
                                                                       Int64 taskId,
                                                                       Int32 processOrder);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Start")]
      private static extern Status BarelyPerformer_Start(IntPtr handle, Int64 performerId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Stop")]
      private static extern Status BarelyPerformer_Stop(IntPtr handle, Int64 performerId);

      [DllImport(pluginName, EntryPoint = "BarelyPercussionInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelyPercussionInstrument_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySamplerInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySamplerInstrument_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySynthInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySynthInstrument_GetDefinition();
    }
  }
}
