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
    /// @param action Task action.
    /// @param dspTime Time in seconds.
    public static void ScheduleTask(Action action, double dspTime) {
      Internal.Musician_CreateOneOffTask(action, dspTime);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Invalid identifier.
      public const Int64 InvalidId = 0;

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
        switch (instrument) {
          case SamplerInstrument sampler:
            _instrumentDefinition = BarelySamplerInstrument_GetDefinition();
            break;
          case SynthInstrument synth:
            _instrumentDefinition = BarelySynthInstrument_GetDefinition();
            break;
          default:
            Debug.LogError("Unsupported instrument type: " + instrument.GetType());
            return InvalidId;
        }
        Status status = BarelyInstrument_Create(Handle, _instrumentDefinition,
                                                AudioSettings.outputSampleRate, _int64Ptr);
        if (!IsOk(status)) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "': " + status);
          return InvalidId;
        }
        Int64 instrumentId = Marshal.ReadInt64(_int64Ptr);
        BarelyInstrument_SetControlEventCallback(
            _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(controlEventCallback),
            IntPtr.Zero);
        BarelyInstrument_SetNoteControlEventCallback(
            _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(noteControlEventCallback),
            IntPtr.Zero);
        BarelyInstrument_SetNoteOffEventCallback(
            _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(noteOffEventCallback),
            IntPtr.Zero);
        BarelyInstrument_SetNoteOnEventCallback(
            _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(noteOnEventCallback),
            IntPtr.Zero);
        return instrumentId;
      }

      /// Destroys an instrument.
      ///
      /// @param instrument Instrument.
      public static void Instrument_Destroy(Instrument instrument) {
        Status status = BarelyInstrument_Destroy(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy instrument '" + instrument.name + "': " + status);
        }
      }

      /// Returns the value of an instrument control.
      ///
      /// @param instrument Instrument.
      /// @param index Control index.
      /// @return Control value.
      public static double Instrument_GetControl(Instrument instrument, int index) {
        Status status = BarelyInstrument_GetControl(Handle, instrument.Id, index, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument control " + index + " value for '" +
                         instrument.name + "': " + status);
        }
        return 0.0;
      }

      /// Returns the value of an instrument note control.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @return Note control value.
      public static double Instrument_GetNoteControl(Instrument instrument, double pitch,
                                                     int index) {
        Status status =
            BarelyInstrument_GetNoteControl(Handle, instrument.Id, pitch, index, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument note pitch " + pitch + " control " + index +
                         " value for '" + instrument.name + "': " + status);
        }
        return 0.0;
      }

      /// Returns whether an instrument note is on or not.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @return True if on, false otherwise.
      public static bool Instrument_IsNoteOn(Instrument instrument, double pitch) {
        Status status = BarelyInstrument_IsNoteOn(Handle, instrument.Id, pitch, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get if instrument note pitch " + pitch + " is on for '" +
                         instrument.name + "': " + status);
        }
        return false;
      }

      /// Processes instrument output samples.
      ///
      /// @param instrument Instrument.
      /// @param outputSamples Output samples.
      /// @param outputChannelCount Number of output channels.
      public static void Instrument_Process(Instrument instrument, float[] outputSamples,
                                            int outputChannelCount) {
        Status status = BarelyInstrument_Process(
            Handle, instrument.Id, _outputSamples, outputChannelCount,
            outputSamples.Length / outputChannelCount, AudioSettings.dspTime);
        if (IsOk(status)) {
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = (float)_outputSamples[i];
          }
        } else {
          if (_handle != IntPtr.Zero) {
            Debug.LogError("Failed to process instrument '" + instrument.name + "': " + status);
          }
          for (int i = 0; i < outputSamples.Length; ++i) {
            outputSamples[i] = 0.0f;
          }
        }
      }

      /// Resets all instrument controls to their default values.
      ///
      /// @param instrument Instrument.
      public static void Instrument_ResetAllControls(Instrument instrument) {
        Status status = BarelyInstrument_ResetAllControls(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument controls for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Resets all instrument note controls to their default values.
      ///
      /// @param instrument Instrument.
      public static void Instrument_ResetAllNoteControls(Instrument instrument, double pitch) {
        Status status = BarelyInstrument_ResetAllNoteControls(Handle, instrument.Id, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument note pitch " + pitch + " controls for '" +
                         instrument.name + "': " + status);
        }
      }

      /// Resets an instrument control to its default value.
      ///
      /// @param instrument Instrument.
      /// @param index Control index.
      public static void Instrument_ResetControl(Instrument instrument, int index) {
        Status status = BarelyInstrument_ResetControl(Handle, instrument.Id, index);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument control " + index + " for '" +
                         instrument.name + "': " + status);
        }
      }

      /// Resets an instrument note control to its default value.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      public static void Instrument_ResetNoteControl(Instrument instrument, double pitch,
                                                     int index) {
        Status status = BarelyInstrument_ResetNoteControl(Handle, instrument.Id, pitch, index);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument note pitch " + pitch + " control " + index +
                         " for '" + instrument.name + "': " + status);
        }
      }

      /// Sets all instrument notes off.
      ///
      /// @param instrument Instrument.
      /// @return True if success, false otherwise.
      public static void Instrument_SetAllNotesOff(Instrument instrument) {
        Status status = BarelyInstrument_SetAllNotesOff(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Sets an instrument control value.
      ///
      /// @param instrument Instrument.
      /// @param index Control index.
      /// @param value Control value.
      /// @param slopePerBeat Control slope in value change per beat.
      public static void Instrument_SetControl(Instrument instrument, int index, double value,
                                               double slopePerBeat) {
        Status status =
            BarelyInstrument_SetControl(Handle, instrument.Id, index, value, slopePerBeat);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument control " + index + " value to " + value +
                         " with slope " + slopePerBeat + " for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Sets instrument data.
      ///
      /// @param instrument Instrument.
      /// @param data Data.
      public static void Instrument_SetData(Instrument instrument, byte[] data) {
        Status status = BarelyInstrument_SetData(Handle, instrument.Id, data, data.Length);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument data to " + data + " for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Sets an instrument note control value.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @param index Note control index.
      /// @param value Note control value.
      /// @param slopePerBeat Note control slope in value change per beat.
      public static void Instrument_SetNoteControl(Instrument instrument, double pitch, int index,
                                                   double value, double slopePerBeat) {
        Status status = BarelyInstrument_SetNoteControl(Handle, instrument.Id, pitch, index, value,
                                                        slopePerBeat);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument note pitch " + pitch + " control " + index +
                         " value to " + value + " with slope " + slopePerBeat + " for '" +
                         instrument.name + "': " + status);
        }
      }

      /// Sets an instrument note off.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @return True if success, false otherwise.
      public static void Instrument_SetNoteOff(Instrument instrument, double pitch) {
        Status status = BarelyInstrument_SetNoteOff(Handle, instrument.Id, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + " for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Sets an instrument note on.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      public static void Instrument_SetNoteOn(Instrument instrument, double pitch,
                                              double intensity) {
        Status status = BarelyInstrument_SetNoteOn(Handle, instrument.Id, pitch, intensity);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity for '" + instrument.name + "': " + status);
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

      /// Creates a new one-off musician task.
      ///
      /// @param task Task action.
      /// @param timestamp Task timestamp in seconds.
      public static void Musician_CreateOneOffTask(Action action, double timestamp) {
        if (timestamp <= Musician_GetTimestamp()) {
          action?.Invoke();
          return;
        }
        List<Action> tasks = null;
        if (_tasks != null && !_tasks.TryGetValue(timestamp, out tasks)) {
          tasks = new List<Action>();
          _tasks.Add(timestamp, tasks);
        }
        tasks?.Add(action);
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
        if (!IsOk(status)) {
          Debug.LogError("Failed to create performer '" + performer.name + "': " + status);
          return InvalidId;
        }
        Int64 performerId = Marshal.ReadInt64(_int64Ptr);
        return performerId;
      }

      /// Creates a new performer task.
      ///
      /// @param performer Performer.
      /// @return Task identifier.
      public static Int64 Performer_CreateTask(Performer performer, Performer.TaskType type,
                                               Action processCallback, double position,
                                               int processOrder) {
        _taskDefinition.processCallback = Marshal.GetFunctionPointerForDelegate(processCallback);
        Status status = Status.UNIMPLEMENTED;
        switch (type) {
          case Performer.TaskType.ONE_OFF:
            status =
                BarelyPerformer_CreateOneOffTask(Handle, performer.Id, _taskDefinition, position,
                                                 processOrder, IntPtr.Zero, _int64Ptr);
            break;
          case Performer.TaskType.RECURRING:
            status =
                BarelyPerformer_CreateRecurringTask(Handle, performer.Id, _taskDefinition, position,
                                                    processOrder, IntPtr.Zero, _int64Ptr);
            break;
        }
        if (!IsOk(status)) {
          Debug.LogError("Failed to create performer '" + performer.name + "': " + status);
          return InvalidId;
        }
        return Marshal.ReadInt64(_int64Ptr);
      }

      /// Destroys a performer.
      ///
      /// @param performer Performer.
      public static void Performer_Destroy(Performer performer) {
        Status status = BarelyPerformer_Destroy(Handle, performer.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy performer '" + performer.name + "': " + status);
        }
      }

      /// Destroys a performer task.
      ///
      /// @param task Task.
      public static void Performer_DestroyTask(Performer.Task task) {
        Status status = BarelyPerformer_DestroyTask(Handle, task.Performer.Id, task.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy performer task '" + task.Id + "': " + status);
        }
      }

      /// Returns the loop begin position of a performer.
      ///
      /// @param performer Performer.
      /// @return Loop begin position in beats.
      public static double Performer_GetLoopBeginPosition(Performer performer) {
        Status status = BarelyPerformer_GetLoopBeginPosition(Handle, performer.Id, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop begin position: " + status);
        }
        return 0.0;
      }

      /// Returns the loop length of a performer.
      ///
      /// @param performer Performer.
      /// @return Loop length in beats.
      public static double Performer_GetLoopLength(Performer performer) {
        Status status = BarelyPerformer_GetLoopLength(Handle, performer.Id, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer loop length: " + status);
        }
        return 0.0;
      }

      /// Returns the position of a performer.
      ///
      /// @param performer Performer.
      /// @return Position in beats.
      public static double Performer_GetPosition(Performer performer) {
        Status status = BarelyPerformer_GetPosition(Handle, performer.Id, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer position: " + status);
        }
        return 0.0;
      }

      /// Returns the position of a performer task.
      ///
      /// @param task Task.
      /// @return Position in beats.
      public static double Performer_GetTaskPosition(Performer.Task task) {
        Status status =
            BarelyPerformer_GetTaskPosition(Handle, task.Performer.Id, task.Id, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task position: " + status);
        }
        return 0.0;
      }

      /// Returns the process order of a performer task.
      ///
      /// @param task Task.
      /// @return Process order.
      public static int Performer_GetTaskProcessOrder(Performer.Task task) {
        Status status =
            BarelyPerformer_GetTaskProcessOrder(Handle, task.Performer.Id, task.Id, _int32Ptr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Int32>(_int32Ptr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer task process order: " + status);
        }
        return 0;
      }

      /// Returns whether a performer is looping or not.
      ///
      /// @param performer Performer.
      /// @return True if looping, false otherwise.
      public static bool Performer_IsLooping(Performer performer) {
        Status status = BarelyPerformer_IsLooping(Handle, performer.Id, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer looping: " + status);
        }
        return false;
      }

      /// Returns whether a performer is playing or not.
      ///
      /// @param performer Performer.
      /// @return True if playing, false otherwise.
      public static bool Performer_IsPlaying(Performer performer) {
        Status status = BarelyPerformer_IsPlaying(Handle, performer.Id, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get performer playing: " + status);
        }
        return false;
      }

      /// Sets the loop begin position of a performer.
      ///
      /// @param performer Performer.
      /// @param loopBeginPosition Loop begin position in beats.
      public static void Performer_SetLoopBeginPosition(Performer performer,
                                                        double loopBeginPosition) {
        Status status =
            BarelyPerformer_SetLoopBeginPosition(Handle, performer.Id, loopBeginPosition);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop begin position: " + status);
        }
      }

      /// Sets the loop length of a performer.
      ///
      /// @param performer Performer.
      /// @param loopLength Loop length in beats.
      public static void Performer_SetLoopLength(Performer performer, double loopLength) {
        Status status = BarelyPerformer_SetLoopLength(Handle, performer.Id, loopLength);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer loop length: " + status);
        }
      }

      /// Sets whether a performer is looping or not.
      ///
      /// @param performer Performer.
      /// @param isLooping True if looping, false otherwise.
      public static void Performer_SetLooping(Performer performer, bool isLooping) {
        Status status = BarelyPerformer_SetLooping(Handle, performer.Id, isLooping);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer looping: " + status);
        }
      }

      /// Sets the position of a performer.
      ///
      /// @param performer Performer.
      /// @param position Position in beats.
      public static void Performer_SetPosition(Performer performer, double position) {
        Status status = BarelyPerformer_SetPosition(Handle, performer.Id, position);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer position: " + status);
        }
      }

      /// Sets the position of a performer task.
      ///
      /// @param task Task.
      /// @param position Position in beats.
      public static void Performer_SetTaskPosition(Performer.Task task, double position) {
        Status status =
            BarelyPerformer_SetTaskPosition(Handle, task.Performer.Id, task.Id, position);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task position: " + status);
        }
      }

      /// Sets the process order of a performer task.
      ///
      /// @param task Task.
      /// @param processOrder Process order.
      public static void Performer_SetTaskProcessOrder(Performer.Task task, int processOrder) {
        Status status =
            BarelyPerformer_SetTaskProcessOrder(Handle, task.Performer.Id, task.Id, processOrder);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set performer task process order: " + status);
        }
      }

      /// Starts a performer.
      ///
      /// @param performer Performer.
      public static void Performer_Start(Performer performer) {
        Status status = BarelyPerformer_Start(Handle, performer.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to start performer: " + status);
        }
      }

      /// Stops a performer.
      ///
      /// @param performer Performer.
      public static void Performer_Stop(Performer performer) {
        Status status = BarelyPerformer_Stop(Handle, performer.Id);
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

      // Task definition.
      [StructLayout(LayoutKind.Sequential)]
      private struct TaskDefinition {
        // Create callback.
        public IntPtr createCallback;

        // Destroy callback.
        public IntPtr destroyCallback;

        // Process callback.
        public IntPtr processCallback;
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

      // Map of scheduled tasks by their timestamps.
      private static Dictionary<double, List<Action>> _tasks = null;

      // Instrument definition.
      private static InstrumentDefinition _instrumentDefinition = new InstrumentDefinition();

      // Task definition.
      private static TaskDefinition _taskDefinition = new TaskDefinition();

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
          while (_tasks.Count > 0) {
            double timestamp = _tasks.ElementAt(0).Key;
            if (timestamp >= dspTime) {
              break;
            }
            BarelyMusician_Update(_handle, timestamp);
            var tasks = _tasks.ElementAt(0).Value;
            for (int i = 0; i < tasks.Count; ++i) {
              tasks[i]?.Invoke();
            }
            _tasks.Remove(timestamp);
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
          _tasks = new Dictionary<double, List<Action>>();
          BarelyMusician_Update(_handle, AudioSettings.dspTime + _latency);
        }

        // Shuts down the native state.
        private void Shutdown() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
          _tasks = null;
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
      private static extern Status BarelyInstrument_SetControlEventCallback(IntPtr handle,
                                                                            Int64 instrumentId,
                                                                            IntPtr callback,
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
      private static extern Status BarelyInstrument_SetNoteControlEventCallback(IntPtr handle,
                                                                                Int64 instrumentId,
                                                                                IntPtr callback,
                                                                                IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern Status BarelyInstrument_SetNoteOff(IntPtr handle, Int64 instrumentId,
                                                               double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffEventCallback")]
      private static extern Status BarelyInstrument_SetNoteOffEventCallback(IntPtr handle,
                                                                            Int64 instrumentId,
                                                                            IntPtr callback,
                                                                            IntPtr userData);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern Status BarelyInstrument_SetNoteOn(IntPtr handle, Int64 instrumentId,
                                                              double pitch, double intensity);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnEventCallback")]
      private static extern Status BarelyInstrument_SetNoteOnEventCallback(IntPtr handle,
                                                                           Int64 instrumentId,
                                                                           IntPtr callback,
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

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CreateOneOffTask")]
      private static extern Status BarelyPerformer_CreateOneOffTask(
          IntPtr handle, Int64 performerId, TaskDefinition definition, double position,
          Int32 processOrder, IntPtr userData, IntPtr outTaskId);

      [DllImport(pluginName, EntryPoint = "BarelyPerformer_CreateRecurringTask")]
      private static extern Status BarelyPerformer_CreateRecurringTask(
          IntPtr handle, Int64 performerId, TaskDefinition definition, double position,
          Int32 processOrder, IntPtr userData, IntPtr outTaskId);

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
