using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Class that wraps musician.
  public static class Musician {
    /// Playback tempo in bpm.
    public static double Tempo {
      get { return _tempo; }
      set {
        value = Math.Max(value, 0.0);
        if (_tempo != value) {
          Native.Musician_SetTempo(value);
          _tempo = value;
        }
      }
    }
    private static double _tempo = 120.0;

    /// Timestamp in seconds.
    public static double Timestamp {
      get { return Native.Musician_GetTimestamp(); }
    }

    /// Returns beats from seconds.
    ///
    /// @param seconds Number of seconds.
    /// @return Number of beats.
    public static double GetBeats(double seconds) {
      return Native.Musician_GetBeats(seconds);
    }

    /// Returns seconds from beats.
    ///
    /// @param beats Number of beats.
    /// @return Number of seconds.
    public static double GetSeconds(double beats) {
      return Native.Musician_GetSeconds(beats);
    }

    /// Schedules task at time.
    ///
    /// @param dspTime Time in seconds.
    /// @param task Task.
    public static void ScheduleTask(double dspTime, Action task) {
      Native.Musician_ScheduleTask(dspTime, task);
    }

    /// Class that wraps native api.
    public static class Native {
      /// Invalid identifier.
      public const Int64 InvalidId = -1;

      /// Creates new instrument.
      ///
      /// @param instrument Instrument.
      /// @param noteOffCallback Reference to note off callback.
      /// @param noteOnCallback Reference to note on callback.
      /// @return Instrument identifier.
      public static Int64 Instrument_Create(Instrument instrument,
                                            Instrument.NoteOffCallback noteOffCallback,
                                            Instrument.NoteOnCallback noteOnCallback) {
        Int64 instrumentId = InvalidId;
        Int32 frameRate = AudioSettings.outputSampleRate;
        Status status = Status.UNIMPLEMENTED;
        switch (instrument) {
          case SynthInstrument synth:
            status =
                BarelyUnityInstrument_Create(Handle, InstrumentType.SYNTH, frameRate, _int64Ptr);
            break;
          default:
            Debug.LogError("Unsupported instrument type: " + instrument.GetType());
            return InvalidId;
        }
        if (IsOk(status)) {
          instrumentId = Marshal.ReadInt64(_int64Ptr);
          BarelyUnityInstrument_SetNoteOffCallback(
              _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(noteOffCallback));
          BarelyUnityInstrument_SetNoteOnCallback(
              _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(noteOnCallback));
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "': " + status);
        }
        return instrumentId;
      }

      /// Destroys instrument.
      ///
      /// @param instrument Instrument.
      public static void Instrument_Destroy(Instrument instrument) {
        Status status = BarelyInstrument_Destroy(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy instrument '" + instrument.name + "': " + status);
        }
      }

      /// Returns instrument parameter value.
      ///
      /// @param instrument Instrument.
      /// @param index Parameter index.
      /// @return Parameter value.
      public static double Instrument_GetParameter(Instrument instrument, int index) {
        Status status = BarelyInstrument_GetParameter(Handle, instrument.Id, index, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument parameter " + index + " value for '" +
                         instrument.name + "': " + status);
        }
        return 0.0;
      }

      /// Returns whether instrument note is playing or not.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitchk.
      /// @return True if playing, false otherwise.
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

      /// Processes next instrument output buffer.
      ///
      /// @param instrument Instrument.
      /// @param output Output buffer.
      /// @param numOutputChannels Number of output channels.
      public static void Instrument_Process(Instrument instrument, float[] output,
                                            int numOutputChannels) {
        Status status =
            BarelyInstrument_Process(Handle, instrument.Id, _output, numOutputChannels,
                                     output.Length / numOutputChannels, AudioSettings.dspTime);
        if (IsOk(status)) {
          for (int i = 0; i < output.Length; ++i) {
            output[i] = (float)_output[i];
          }
        } else {
          if (_handle != IntPtr.Zero) {
            Debug.LogError("Failed to process instrument '" + instrument.name + "': " + status);
          }
          for (int i = 0; i < output.Length; ++i) {
            output[i] = 0.0f;
          }
        }
      }

      /// Resets all instrument parameters to default value.
      ///
      /// @param instrument Instrument.
      public static void Instrument_ResetAllParameters(Instrument instrument) {
        Status status = BarelyInstrument_ResetAllParameters(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument parameters for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Resets instrument parameter to default value.
      ///
      /// @param instrument Instrument.
      /// @param index Parameter index.
      public static void Instrument_ResetParameter(Instrument instrument, int index) {
        Status status = BarelyInstrument_ResetParameter(Handle, instrument.Id, index);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument parameter " + index + " for '" +
                         instrument.name + "': " + status);
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

      /// Sets instrument parameter value.
      ///
      /// @param instrument Instrument.
      /// @param index Parameter index.
      /// @param value Parameter value.
      /// @param slope Parameter slope in value change per second.
      public static void Instrument_SetParameter(Instrument instrument, int index, double value,
                                                 double slope) {
        Status status = BarelyInstrument_SetParameter(Handle, instrument.Id, index, value, slope);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument parameter " + index + " value to " + value +
                         " with slope " + slope + " for '" + instrument.name + "': " + status);
        }
      }

      /// Starts instrument note.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @param intensity Note intensity.
      public static void Instrument_StartNote(Instrument instrument, double pitch,
                                              double intensity) {
        Status status = BarelyInstrument_StartNote(Handle, instrument.Id, pitch, intensity);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " with " + intensity +
                         " intensity for '" + instrument.name + "': " + status);
        }
      }

      /// Stops all instrument notes.
      ///
      /// @param instrument Instrument.
      /// @return True if success, false otherwise.
      public static void Instrument_StopAllNotes(Instrument instrument) {
        Status status = BarelyInstrument_StopAllNotes(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Stops instrument note.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @return True if success, false otherwise.
      public static void Instrument_StopNote(Instrument instrument, double pitch) {
        Status status = BarelyInstrument_StopNote(Handle, instrument.Id, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + " for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Returns musician beats from seconds.
      ///
      /// @param seconds Number of seconds.
      /// @return Number of beats.
      public static double Musician_GetBeats(double seconds) {
        Status status = BarelyMusician_GetBeats(Handle, seconds, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician beats from seconds: " + status);
        }
        return 0.0;
      }

      /// Returns musician seconds from beats.
      ///
      /// @param beats Number of beats.
      /// @return Number of seconds.
      public static double Musician_GetSeconds(double beats) {
        Status status = BarelyMusician_GetSeconds(Handle, beats, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician seconds from beats: " + status);
        }
        return 0.0;
      }

      /// Returns musician timestamp.
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
      /// @param tempo Tempo in bpm.
      public static void Musician_SetTempo(double tempo) {
        Status status = BarelyMusician_SetTempo(Handle, tempo);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo: " + status);
        }
      }

      // Instrument types.
      private enum InstrumentType {
        // Percussion instrument.
        PERCUSSION = 0,
        // Synth instrument.
        SYNTH = 1,
      }

      // Status codes.
      private enum Status {
        // Success.
        OK = 0,
        // Invalid argument error.
        INVALID_ARGUMENT = 1,
        // Not found error.
        NOT_FOUND = 2,
        // Already exists error.
        ALREADY_EXISTS = 3,
        // Unimplemented error.
        UNIMPLEMENTED = 4,
        // Internal error.
        INTERNAL = 5,
        // Unknown error.
        UNKNOWN = 6,
      }

      // Returns whether status is okay or not.
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

      // Internal output buffer.
      private static double[] _output = null;

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
          _output = new double[config.dspBufferSize * (int)config.speakerMode];
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

      [DllImport(pluginName, EntryPoint = "BarelyUnityInstrument_Create")]
      private static extern Status BarelyUnityInstrument_Create(IntPtr handle, InstrumentType type,
                                                                Int32 frameRate,
                                                                IntPtr outInstrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern Status BarelyInstrument_Destroy(IntPtr handle, Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetParameter")]
      private static extern Status BarelyInstrument_GetParameter(IntPtr handle, Int64 instrumentId,
                                                                 Int32 index, IntPtr outValue);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
      private static extern Status BarelyInstrument_IsNoteOn(IntPtr handle, Int64 instrumentId,
                                                             double pitch, IntPtr outIsNoteOn);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
      private static extern Status BarelyInstrument_Process(IntPtr handle, Int64 instrumentId,
                                                            [In, Out] double[] output,
                                                            Int32 numOutputChannels,
                                                            Int32 numOutputFrames,
                                                            double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllParameters")]
      private static extern Status BarelyInstrument_ResetAllParameters(IntPtr handle,
                                                                       Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetParameter")]
      private static extern Status BarelyInstrument_ResetParameter(IntPtr handle,
                                                                   Int64 instrumentId, Int32 index);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
      private static extern Status BarelyInstrument_SetData(IntPtr handle, Int64 instrumentId,
                                                            [In] byte[] data, Int32 size);

      [DllImport(pluginName, EntryPoint = "BarelyUnityInstrument_SetNoteOffCallback")]
      private static extern Status BarelyUnityInstrument_SetNoteOffCallback(IntPtr handle,
                                                                            Int64 instrumentId,
                                                                            IntPtr callback);

      [DllImport(pluginName, EntryPoint = "BarelyUnityInstrument_SetNoteOnCallback")]
      private static extern Status BarelyUnityInstrument_SetNoteOnCallback(IntPtr handle,
                                                                           Int64 instrumentId,
                                                                           IntPtr callback);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetParameter")]
      private static extern Status BarelyInstrument_SetParameter(IntPtr handle, Int64 instrumentId,
                                                                 Int32 index, double value,
                                                                 double slope);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_StartNote")]
      private static extern Status BarelyInstrument_StartNote(IntPtr handle, Int64 instrumentId,
                                                              double pitch, double intensity);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopAllNotes")]
      private static extern Status BarelyInstrument_StopAllNotes(IntPtr handle, Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopNote")]
      private static extern Status BarelyInstrument_StopNote(IntPtr handle, Int64 instrumentId,
                                                             double pitch);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
      private static extern Status BarelyMusician_Create(IntPtr outHandle);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
      private static extern Status BarelyMusician_Destroy(IntPtr handle);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetBeats")]
      private static extern Status BarelyMusician_GetBeats(IntPtr handle, double seconds,
                                                           IntPtr outBeats);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetSeconds")]
      private static extern Status BarelyMusician_GetSeconds(IntPtr handle, double beats,
                                                             IntPtr outSeconds);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
      private static extern Status BarelyMusician_GetTimestamp(IntPtr handle, IntPtr outTimestamp);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
      private static extern Status BarelyMusician_SetTempo(IntPtr handle, double tempo);

      [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
      private static extern Status BarelyMusician_Update(IntPtr handle, double timestamp);
    }
  }
}
