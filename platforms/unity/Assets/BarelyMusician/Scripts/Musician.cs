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

      /// Task definition.
      [StructLayout(LayoutKind.Sequential)]
      public class TaskDefinition {
        /// Create callback.
        public Action createCallback;

        /// Destroy callback.
        public Action destroyCallback;

        /// Process callback.
        public Action processCallback;
      }

      /// Creates a new instrument.
      ///
      /// @param instrument Instrument.
      /// @param controlEventCallback Control event callback.
      /// @param noteControlEventCallback Note control event callback.
      /// @param noteOffEventCallback Note off event callback.
      /// @param noteOnEventCallback Note on event callback.
      /// @return Instrument identifier.
      public static Int64 Instrument_Create(
          Instrument instrument, Instrument.ControlEventCallback controlEventCallback,
          Instrument.NoteControlEventCallback noteControlEventCallback,
          Instrument.NoteOffEventCallback noteOffEventCallback,
          Instrument.NoteOnEventCallback noteOnEventCallback) {
        InstrumentDefinition definition;
        switch (instrument) {
          case SamplerInstrument sampler:
            definition = BarelySamplerInstrument_GetDefinition();
            break;
          case SynthInstrument synth:
            definition = BarelySynthInstrument_GetDefinition();
            break;
          default:
            Debug.LogError("Unsupported instrument type: " + instrument.GetType());
            return InvalidId;
        }
        Status status =
            BarelyInstrument_Create(Handle, definition, AudioSettings.outputSampleRate, _int64Ptr);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "': " + status);
          return InvalidId;
        }
        Int64 instrumentId = Marshal.ReadInt64(_int64Ptr);
        BarelyInstrument_SetControlEventCallback(_handle, instrumentId, controlEventCallback,
                                                 IntPtr.Zero);
        BarelyInstrument_SetNoteControlEventCallback(_handle, instrumentId,
                                                     noteControlEventCallback, IntPtr.Zero);
        BarelyInstrument_SetNoteOffEventCallback(_handle, instrumentId, noteOffEventCallback,
                                                 IntPtr.Zero);
        BarelyInstrument_SetNoteOnEventCallback(_handle, instrumentId, noteOnEventCallback,
                                                IntPtr.Zero);
        return instrumentId;
      }

      /// Destroys an instrument.
      ///
      /// @param instrumentId Instrument identifier.
      public static void Instrument_Destroy(Int64 instrumentId) {
        Status status = BarelyInstrument_Destroy(Handle, instrumentId);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy instrument '" + instrumentId + "': " + status);
        }
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param index Control index.
      /// @return Control value.
      public static double Instrument_GetControl(Int64 instrumentId, int index) {
        Status status = BarelyInstrument_GetControl(Handle, instrumentId, index, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to get instrument control " + index + " value for '" +
                         instrumentId + "': " + status);
        }
        return 0.0;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @return Note control value.
      public static double Instrument_GetNoteControl(Int64 instrumentId, double pitch, int index) {
        Status status =
            BarelyInstrument_GetNoteControl(Handle, instrumentId, pitch, index, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to get instrument note pitch " + pitch + " control " + index +
                         " value for '" + instrumentId + "': " + status);
        }
        return 0.0;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(Int64 instrumentId, double pitch) {
        Status status = BarelyInstrument_IsNoteOn(Handle, instrumentId, pitch, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to get if instrument note pitch " + pitch + " is on for '" +
                         instrumentId + "': " + status);
        }
        return false;
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
          if (_handle != IntPtr.Zero && instrumentId != InvalidId) {
            Debug.LogError("Failed to process instrument with id " + instrumentId + ": " + status);
          }
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
          Debug.LogError("Failed to reset all instrument controls for '" + instrumentId +
                         "': " + status);
        }
      }

      /// Resets all instrument note control values.
      ///
      /// @param instrumentId Instrument identifier.
      public static void Instrument_ResetAllNoteControls(Int64 instrumentId, double pitch) {
        Status status = BarelyInstrument_ResetAllNoteControls(Handle, instrumentId, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to reset all instrument note pitch " + pitch + " controls for '" +
                         instrumentId + "': " + status);
        }
      }

      /// Resets an instrument control value.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param index Control index.
      public static void Instrument_ResetControl(Int64 instrumentId, int index) {
        Status status = BarelyInstrument_ResetControl(Handle, instrumentId, index);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to reset instrument control " + index + " for '" + instrumentId +
                         "': " + status);
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
                         " for '" + instrumentId + "': " + status);
        }
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrumentId Instrument identifier.
      /// @return True if success, false otherwise.
      public static void Instrument_SetAllNotesOff(Int64 instrumentId) {
        Status status = BarelyInstrument_SetAllNotesOff(Handle, instrumentId);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to stop all instrument notes for '" + instrumentId +
                         "': " + status);
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
                         " with slope " + slopePerBeat + " for '" + instrumentId + "': " + status);
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrumentId Instrument identifier.
      /// @param data Data.
      public static void Instrument_SetData(Int64 instrumentId, byte[] data) {
        Status status = BarelyInstrument_SetData(Handle, instrumentId, data, data.Length);
        if (!IsOk(status) && _handle != IntPtr.Zero && instrumentId != InvalidId) {
          Debug.LogError("Failed to set instrument data to " + data + " for '" + instrumentId +
                         "': " + status);
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
                         " value to " + value + " with slope " + slopePerBeat + " for '" +
                         instrumentId + "': " + status);
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
          Debug.LogError("Failed to stop instrument note " + pitch + " for '" + instrumentId +
                         "': " + status);
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
                         " intensity for '" + instrumentId + "': " + status);
        }
      }

      /// Returns the tempo of a musician.
      ///
      /// @return Tempo in beats per minute.
      public static double Musician_GetTempo() {
        Status status = BarelyMusician_GetTempo(Handle, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo: " + status);
        }
        return 0.0;
      }

      /// Returns the timestamp of a musician.
      ///
      /// @return Timestamp in seconds.
      public static double Musician_GetTimestamp() {
        Status status = BarelyMusician_GetTimestamp(Handle, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician timestamp: " + status);
        }
        return 0.0;
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
        if (_taskCallbacks != null && !_taskCallbacks.TryGetValue(timestamp, out callbacks)) {
          callbacks = new List<Action>();
          _taskCallbacks.Add(timestamp, callbacks);
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
      /// @return Performer identifier.
      public static Int64 Performer_Create(Performer performer) {
        Status status = BarelyPerformer_Create(Handle, _int64Ptr);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to create performer '" + performer.name + "': " + status);
          return InvalidId;
        }
        Int64 performerId = Marshal.ReadInt64(_int64Ptr);
        return performerId;
      }

      /// Creates a new performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param definition Task definition.
      /// @param isOneOff True if one off task, false otherwise.
      /// @param position Task position.
      /// @param processOrder Task process order.
      /// @return Task identifier.
      public static Int64 Performer_CreateTask(Int64 performerId, TaskDefinition definition,
                                               bool isOneOff, double position, int processOrder) {
        Status status = BarelyPerformer_CreateTask(Handle, performerId, definition, isOneOff,
                                                   position, processOrder, IntPtr.Zero, _int64Ptr);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to create performer '" + performerId + "': " + status);
          return InvalidId;
        }
        return Marshal.ReadInt64(_int64Ptr);
      }

      /// Destroys a performer.
      ///
      /// @param performerId Performer identifier.
      public static void Performer_Destroy(Int64 performerId) {
        Status status = BarelyPerformer_Destroy(Handle, performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to destroy performer '" + performerId + "': " + status);
        }
      }

      /// Destroys a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      public static void Performer_DestroyTask(Int64 performerId, Int64 taskId) {
        Status status = BarelyPerformer_DestroyTask(Handle, performerId, taskId);
        if (!IsOk(status) && _handle != IntPtr.Zero && performerId != InvalidId &&
            taskId != InvalidId) {
          Debug.LogError("Failed to destroy performer task '" + taskId + "': " + status);
        }
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(Int64 performerId) {
        Status status = BarelyPerformer_GetLoopBeginPosition(Handle, performerId, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer loop begin position: " + status);
        }
        return 0.0;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(Int64 performerId) {
        Status status = BarelyPerformer_GetLoopLength(Handle, performerId, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer loop length: " + status);
        }
        return 0.0;
      }

      /// Returns the position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @return Position in beats.
      public static double Performer_GetPosition(Int64 performerId) {
        Status status = BarelyPerformer_GetPosition(Handle, performerId, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero && performerId != InvalidId) {
          Debug.LogError("Failed to get performer position: " + status);
        }
        return 0.0;
      }

      /// Returns the position of a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      /// @return Position in beats.
      public static double Performer_GetTaskPosition(Int64 performerId, Int64 taskId) {
        Status status = BarelyPerformer_GetTaskPosition(Handle, performerId, taskId, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position: " + status);
        }
        return 0.0;
      }

      /// Returns the process order of a performer task.
      ///
      /// @param performerId Performer identifier.
      /// @param taskId Task identifier.
      /// @return Process order.
      public static int Performer_GetTaskProcessOrder(Int64 performerId, Int64 taskId) {
        Status status = BarelyPerformer_GetTaskProcessOrder(Handle, performerId, taskId, _int32Ptr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Int32>(_int32Ptr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task process order: " + status);
        }
        return 0;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performerId Performer identifier.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(Int64 performerId) {
        Status status = BarelyPerformer_IsLooping(Handle, performerId, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer looping: " + status);
        }
        return false;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performerId Performer identifier.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(Int64 performerId) {
        Status status = BarelyPerformer_IsPlaying(Handle, performerId, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer playing: " + status);
        }
        return false;
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(Int64 performerId,
                                                        double loopBeginPosition) {
        Status status =
            BarelyPerformer_SetLoopBeginPosition(Handle, performerId, loopBeginPosition);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position: " + status);
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(Int64 performerId, double loopLength) {
        Status status = BarelyPerformer_SetLoopLength(Handle, performerId, loopLength);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length: " + status);
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performerId Performer identifier.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(Int64 performerId, bool isLooping) {
        Status status = BarelyPerformer_SetLooping(Handle, performerId, isLooping);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping: " + status);
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performerId Performer identifier.
      /// @param position Position in beats.
      public static void Performer_SetPosition(Int64 performerId, double position) {
        Status status = BarelyPerformer_SetPosition(Handle, performerId, position);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
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
        if (!IsOk(status) && _handle != IntPtr.Zero) {
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
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task process order: " + status);
        }
      }

      /// Starts a performer.
      ///
      /// @param performerId Performer identifier.
      public static void Performer_Start(Int64 performerId) {
        Status status = BarelyPerformer_Start(Handle, performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to start performer: " + status);
        }
      }

      /// Stops a performer.
      ///
      /// @param performerId Performer identifier.
      public static void Performer_Stop(Int64 performerId) {
        Status status = BarelyPerformer_Stop(Handle, performerId);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
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

      /// Control definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct ControlDefinition {
        /// Default value.
        public double defaultValue;

        /// Minimum value.
        public double minValue;

        /// Maximum value.
        public double maxValue;
      }

      // Instrument definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct InstrumentDefinition {
        // Create callback.
        public IntPtr createCallback;

        // Destroy callback.
        public IntPtr destroyCallback;

        // Process callback.
        public IntPtr processCallback;

        // Set control callback.
        public IntPtr setControlCallback;

        // Set data callback.
        public IntPtr setDataCallback;

        // Set note control callback.
        public IntPtr setNoteControlCallback;

        // Set note off callback.
        public IntPtr setNoteOffCallback;

        // Set note on callback.
        public IntPtr setNoteOnCallback;

        // Array of control definitions.
        public IntPtr controlDefinitions;

        // Number of control definitions.
        public Int32 controlDefinitionCount;

        // Array of note control definitions.
        public IntPtr noteControlDefinitions;

        // Number of note control definitions.
        public Int32 noteControlDefinitionCount;
      }

      // Returns whether a status is okay or not.
      private static bool IsOk(Status status) {
        return (status == Status.OK);
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

      // `Boolean` type pointer.
      private static IntPtr _booleanPtr = IntPtr.Zero;

      // `Double` type pointer.
      private static IntPtr _doublePtr = IntPtr.Zero;

      // `Int32` type pointer.
      private static IntPtr _int32Ptr = IntPtr.Zero;

      // `Int64` type pointer.
      private static IntPtr _int64Ptr = IntPtr.Zero;

      // `IntPtr` type pointer.
      private static IntPtr _intPtrPtr = IntPtr.Zero;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Latency in seconds.
      private static double _latency = 0.0;

      // Internal output samples.
      private static double[] _outputSamples = null;

      // Map of scheduled task callbacks by their times.
      private static Dictionary<double, List<Action>> _taskCallbacks = null;

      // Component that manages internal state.
      private class State : MonoBehaviour {
        private void Awake() {
          AllocatePtrs();
          AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
          Initialize();
        }

        private void OnDestroy() {
          AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
          Shutdown();
          DeallocatePtrs();
        }

        private void OnApplicationQuit() {
          Shutdown();
          DeallocatePtrs();
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
          double dspTime = AudioSettings.dspTime + lookahead;
          while (_taskCallbacks.Count > 0) {
            double taskDspTime = _taskCallbacks.ElementAt(0).Key;
            if (taskDspTime > dspTime) {
              break;
            }
            BarelyMusician_Update(_handle, taskDspTime);
            var callbacks = _taskCallbacks.ElementAt(0).Value;
            for (int i = 0; i < callbacks.Count; ++i) {
              callbacks[i]?.Invoke();
            }
            _taskCallbacks.Remove(taskDspTime);
          }
          BarelyMusician_Update(_handle, dspTime);
        }

        // Allocates unmanaged memory for native calls.
        private void AllocatePtrs() {
          _booleanPtr = Marshal.AllocHGlobal(Marshal.SizeOf<Boolean>());
          _doublePtr = Marshal.AllocHGlobal(Marshal.SizeOf<Double>());
          _int32Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int32>());
          _int64Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int64>());
          _intPtrPtr = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        }

        // Deallocates unmanaged memory for native calls.
        private void DeallocatePtrs() {
          if (_booleanPtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_booleanPtr);
            _booleanPtr = IntPtr.Zero;
          }
          if (_doublePtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_doublePtr);
            _doublePtr = IntPtr.Zero;
          }
          if (_int32Ptr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_int32Ptr);
            _int32Ptr = IntPtr.Zero;
          }
          if (_int64Ptr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_int64Ptr);
            _int64Ptr = IntPtr.Zero;
          }
          if (_intPtrPtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_intPtrPtr);
            _intPtrPtr = IntPtr.Zero;
          }
        }

        // Initializes the native state.
        private void Initialize() {
          _isShuttingDown = false;
          Status status = BarelyMusician_Create(_intPtrPtr);
          if (!IsOk(status)) {
            Debug.LogError("Failed to initialize BarelyMusician: " + status);
            return;
          }
          _handle = Marshal.PtrToStructure<IntPtr>(_intPtrPtr);
          BarelyMusician_SetTempo(_handle, _tempo);
          var config = AudioSettings.GetConfiguration();
          _latency = (double)(config.dspBufferSize) / (double)config.sampleRate;
          _outputSamples = new double[config.dspBufferSize * (int)config.speakerMode];
          _taskCallbacks = new Dictionary<double, List<Action>>();
          BarelyMusician_Update(_handle, AudioSettings.dspTime + _latency);
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
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
                                                           Int32 frameRate, IntPtr outInstrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern Status BarelyInstrument_Destroy(IntPtr handle, Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetControl")]
      private static extern Status BarelyInstrument_GetControl(IntPtr handle, Int64 instrumentId,
                                                               Int32 index, IntPtr outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetNoteControl")]
      private static extern Status BarelyInstrument_GetNoteControl(IntPtr handle,
                                                                   Int64 instrumentId, double pitch,
                                                                   Int32 index, IntPtr outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern Status BarelyInstrument_IsNoteOn(IntPtr handle, Int64 instrumentId,
                                                             double pitch, IntPtr outIsNoteOn);

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
          IntPtr handle, Int64 instrumentId, Instrument.ControlEventCallback callback,
          IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
      private static extern Status BarelyInstrument_SetData(IntPtr handle, Int64 instrumentId,
                                                            [In] byte[] data, Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern Status BarelyInstrument_SetNoteControl(IntPtr handle,
                                                                   Int64 instrumentId, double pitch,
                                                                   Int32 index, double value,
                                                                   double slopePerBeat);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteControlEventCallback")]
      private static extern Status BarelyInstrument_SetNoteControlEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument.NoteControlEventCallback callback,
          IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern Status BarelyInstrument_SetNoteOff(IntPtr handle, Int64 instrumentId,
                                                               double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffEventCallback")]
      private static extern Status BarelyInstrument_SetNoteOffEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument.NoteOffEventCallback callback,
          IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern Status BarelyInstrument_SetNoteOn(IntPtr handle, Int64 instrumentId,
                                                              double pitch, double intensity);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnEventCallback")]
      private static extern Status BarelyInstrument_SetNoteOnEventCallback(
          IntPtr handle, Int64 instrumentId, Instrument.NoteOnEventCallback callback,
          IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
      private static extern Status BarelyMusician_Create(IntPtr outHandle);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
      private static extern Status BarelyMusician_Destroy(IntPtr handle);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
      private static extern Status BarelyMusician_GetTempo(IntPtr handle, IntPtr outTempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
      private static extern Status BarelyMusician_GetTimestamp(IntPtr handle, IntPtr outTimestamp);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern Status BarelyMusician_SetTempo(IntPtr handle, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern Status BarelyMusician_Update(IntPtr handle, double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Create")]
      private static extern Status BarelyPerformer_Create(IntPtr handle, IntPtr outPerformerId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CreateTask")]
      private static extern Status BarelyPerformer_CreateTask(IntPtr handle, Int64 performerId,
                                                              TaskDefinition definition,
                                                              bool isOneOff, double position,
                                                              Int32 processOrder, IntPtr userData,
                                                              IntPtr outTaskId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_Destroy")]
      private static extern Status BarelyPerformer_Destroy(IntPtr handle, Int64 performerId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_DestroyTask")]
      private static extern Status BarelyPerformer_DestroyTask(IntPtr handle, Int64 performerId,
                                                               Int64 taskId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopBeginPosition")]
      private static extern Status BarelyPerformer_GetLoopBeginPosition(
          IntPtr handle, Int64 performerId, IntPtr outLoopBeginPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetLoopLength")]
      private static extern Status BarelyPerformer_GetLoopLength(IntPtr handle, Int64 performerId,
                                                                 IntPtr outLoopLength);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern Status BarelyPerformer_GetPosition(IntPtr handle, Int64 performerId,
                                                               IntPtr outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetTaskPosition")]
      private static extern Status BarelyPerformer_GetTaskPosition(IntPtr handle, Int64 performerId,
                                                                   Int64 taskId,
                                                                   IntPtr outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_GetTaskProcessOrder")]
      private static extern Status BarelyPerformer_GetTaskProcessOrder(IntPtr handle,
                                                                       Int64 performerId,
                                                                       Int64 taskId,
                                                                       IntPtr outProcessOrder);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsLooping")]
      private static extern Status BarelyPerformer_IsLooping(IntPtr handle, Int64 performerId,
                                                             IntPtr outIsLooping);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_IsPlaying")]
      private static extern Status BarelyPerformer_IsPlaying(IntPtr handle, Int64 performerId,
                                                             IntPtr outIsPlaying);

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

      [DllImport(pluginName, EntryPoint = "BarelySamplerInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySamplerInstrument_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySynthInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySynthInstrument_GetDefinition();
    }
  }
}
