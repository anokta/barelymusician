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
    /// @param dspTime Time in seconds.
    /// @param task Task.
    public static void ScheduleTask(double dspTime, Action task) {
      Internal.Musician_ScheduleTask(dspTime, task);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Invalid identifier.
      public const Int64 InvalidId = 0;

      /// Instrument definition.
      [StructLayout(LayoutKind.Sequential)]
      public struct InstrumentDefinition {
        /// Create callback.
        IntPtr create_callback;

        /// Destroy callback.
        IntPtr destroy_callback;

        /// Process callback.
        IntPtr process_callback;

        /// Set control callback.
        IntPtr set_control_callback;

        /// Set data callback.
        IntPtr set_data_callback;

        /// Set note control callback.
        IntPtr set_note_control_callback;

        /// Set note off callback.
        IntPtr set_note_off_callback;

        /// Set note on callback.
        IntPtr set_note_on_callback;

        /// Array of control definitions.
        IntPtr control_definitions;

        /// Number of control definitions.
        Int32 control_definition_count;

        /// Array of note control definitions.
        IntPtr note_control_definitions;

        /// Number of note control definitions.
        Int32 note_control_definition_count;
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

      /// Returns the musician tempo.
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

      /// Returns the musician timestamp.
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

      /// Schedules musician task at timestamp.
      ///
      /// @param timestamp Timestamp in seconds.
      /// @param task Task.
      public static void Musician_ScheduleTask(double timestamp, Action task) {
        if (timestamp <= Musician_GetTimestamp()) {
          task?.Invoke();
          return;
        }
        List<Action> tasks = null;
        if (_tasks != null && !_tasks.TryGetValue(timestamp, out tasks)) {
          tasks = new List<Action>();
          _tasks.Add(timestamp, tasks);
        }
        tasks?.Add(task);
      }

      /// Sets musician tempo.
      ///
      /// @param tempo Tempo in beats per minute.
      public static void Musician_SetTempo(double tempo) {
        Status status = BarelyMusician_SetTempo(Handle, tempo);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo: " + status);
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

      [DllImport(pluginName, EntryPoint = "BarelySamplerInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySamplerInstrument_GetDefinition();

      [DllImport(pluginName, EntryPoint = "BarelySynthInstrument_GetDefinition")]
      private static extern InstrumentDefinition BarelySynthInstrument_GetDefinition();
    }
  }
}
