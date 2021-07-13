using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public static class BarelyMusician {
    // Invalid id.
    public const Int64 InvalidId = -1;

    // Note off event.
    public delegate void NoteOffEvent(Instrument instrument, float pitch);
    public static event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(Instrument instrument, float pitch, float intensity);
    public static event NoteOnEvent OnNoteOn;

    // Creates new instrument.
    public static Int64 Create(Instrument instrument) {
      double dspTime = AudioSettings.dspTime;
      Int64 id = InvalidId;
      Type instrumentType = instrument.GetType();
      if (instrumentType == typeof(SynthInstrument)) {
        id = CreateSynthInstrumentNative(InstancePtr, dspTime);
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      if (id != InvalidId) {
        _instruments.Add(id, instrument);
      }
      return id;
    }

    // Destroys instrument.
    public static void Destroy(Instrument instrument) {
      DestroyNative(InstancePtr, instrument.Id, AudioSettings.dspTime);
      _instruments.Remove(instrument.Id);
    }

    // Processes instrument.
    public static void Process(Instrument instrument, float[] output, int numChannels) {
      ProcessNative(InstancePtr, instrument.Id, AudioSettings.dspTime, output, numChannels,
                    output.Length / numChannels);
    }

    // Schedules instrument note off.
    public static bool ScheduleNoteOff(Instrument instrument, double dspTime, float pitch) {
      return SetNoteOffNative(InstancePtr, instrument.Id, dspTime, pitch);
    }

    // Schedules instrument note on.
    public static bool ScheduleNoteOn(Instrument instrument, double dspTime, float pitch, float intensity) {
      return SetNoteOnNative(InstancePtr, instrument.Id, dspTime, pitch, intensity);
    }

    // Sets all instrument notes off.
    public static bool SetAllNotesOff(Instrument instrument) {
      return SetAllNotesOffNative(InstancePtr, instrument.Id, AudioSettings.dspTime);
    }

    // Sets all instrument parameters to default value.
    public static bool SetAllParamsToDefault(Instrument instrument) {
      return SetAllParamsToDefaultNative(InstancePtr, instrument.Id, AudioSettings.dspTime);
    }

    // Sets instrument note off.
    public static bool SetNoteOff(Instrument instrument, float pitch) {
      return SetNoteOffNative(InstancePtr, instrument.Id, AudioSettings.dspTime, pitch);
    }

    // Sets instrument note on.
    public static bool SetNoteOn(Instrument instrument, float pitch, float intensity) {
      return SetNoteOnNative(InstancePtr, instrument.Id, AudioSettings.dspTime, pitch, intensity);
    }

    // Sets instrument parameter value.
    public static bool SetParam(Instrument instrument, int paramId, float value) {
      return SetParamNative(InstancePtr, instrument.Id, AudioSettings.dspTime, paramId, value);
    }

    // Sets instrument parameter value to default.
    public static bool SetParamToDefault(Instrument instrument, int paramId) {
      return SetParamToDefaultNative(InstancePtr, instrument.Id, AudioSettings.dspTime, paramId);
    }

    // Singleton instance.
    private static IntPtr InstancePtr {
      get {
        if (_isShuttingDown) {
          return IntPtr.Zero;
        }
        if (_instancePtr == IntPtr.Zero) {
          var instance = new GameObject() {
            hideFlags = HideFlags.HideAndDontSave
          }.AddComponent<BarelyMusicianInternal>();
          GameObject.DontDestroyOnLoad(instance.gameObject);
          if (_instancePtr == IntPtr.Zero) {
            Debug.LogError("Could not initialize BarelyMusician.");
            GameObject.DestroyImmediate(instance.gameObject);
          }
        }
        return _instancePtr;
      }
    }
    private static IntPtr _instancePtr = IntPtr.Zero;

    // List of instruments.
    private static Dictionary<Int64, Instrument> _instruments = new Dictionary<Int64, Instrument>();

    // Denotes if the system is shutting down to avoid re-initialization.
    private static bool _isShuttingDown = false;

    // Internal component to update the native state.
    private class BarelyMusicianInternal : MonoBehaviour {
      // Log severity.
      private enum LogSeverity {
        Info = 0,
        Warning = 1,
        Error = 2,
        Fatal = 3,
      }

      // Debug callback.
      private delegate void DebugCallback(int severity, string message);
      private DebugCallback _debugCallback = null;

      // Note off callback.
      private delegate void NoteOffCallback(Int64 id, float pitch);
      private NoteOffCallback _noteOffCallback = null;

      // Note on callback.
      private delegate void NoteOnCallback(Int64 id, float pitch, float intensity);
      private NoteOnCallback _noteOnCallback = null;

      private void Awake() {
        AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
        _debugCallback = delegate (int severity, string message) {
          message = "{::" + pluginName + "::}" + message;
          switch ((LogSeverity)severity) {
            case LogSeverity.Info:
              Debug.Log(message);
              break;
            case LogSeverity.Warning:
              Debug.LogWarning(message);
              break;
            case LogSeverity.Error:
              Debug.LogError(message);
              break;
            case LogSeverity.Fatal:
              Debug.LogAssertion(message);
              break;
          }
        };
        _instancePtr =
            InitializeNative(AudioSettings.outputSampleRate, Marshal.GetFunctionPointerForDelegate(_debugCallback));
        _noteOffCallback = delegate (Int64 id, float pitch) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(instrument, pitch);
          } else {
            Debug.LogWarning("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallbackNative(_instancePtr, Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (Int64 id, float pitch, float intensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(instrument, pitch, intensity);
          } else {
            Debug.LogWarning("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallbackNative(_instancePtr, Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
      }

      private void OnDestroy() {
        AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
        ShutdownNative(_instancePtr);
        _instancePtr = IntPtr.Zero;
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        foreach (var instrument in _instruments.Values) {
          SetAllNotesOffNative(_instancePtr, instrument.Id, AudioSettings.dspTime);
        }
        GameObject.Destroy(gameObject);
      }

      private void OnAudioConfigurationChanged(bool deviceWasChanged) {
        SetSampleRateNative(_instancePtr, AudioSettings.dspTime, AudioSettings.outputSampleRate);
        foreach (var instrument in _instruments.Values) {
          instrument.Source?.Play();
        }
      }

      private void LateUpdate() {
        double lookahead = 2.0 * (double)Time.smoothDeltaTime;
        double updateTime = AudioSettings.dspTime + lookahead;
        UpdateNative(_instancePtr, updateTime);
      }
    }

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "BarelyInitialize")]
    private static extern IntPtr InitializeNative(int sampleRate, IntPtr debugCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelyShutdown")]
    private static extern void ShutdownNative(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "BarelyCreateSynthInstrument")]
    private static extern Int64 CreateSynthInstrumentNative(IntPtr instancePtr, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyDestroyInstrument")]
    private static extern void DestroyNative(IntPtr instancePtr, Int64 instrumentId, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyGetInstrumentParam")]
    private static extern float GetParamNative(IntPtr instancePtr, Int64 instrumentId, int param_id);

    [DllImport(pluginName, EntryPoint = "BarelyIsInstrumentNoteOn")]
    private static extern bool IsNoteOnNative(IntPtr instancePtr, Int64 instrumentId, float pitch);

    [DllImport(pluginName, EntryPoint = "BarelyProcessInstrument")]
    private static extern void ProcessNative(IntPtr instancePtr, Int64 instrumentId, double timestamp,
                                             [In, Out] float[] output, int numChannels, int numFrames);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentNotesOff")]
    private static extern bool SetAllNotesOffNative(IntPtr instancePtr, Int64 instrumentId, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentParamsToDefault")]
    private static extern bool SetAllParamsToDefaultNative(IntPtr instancePtr, Int64 instrumentId, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOffCallback")]
    private static extern void SetNoteOffCallbackNative(IntPtr instancePtr, IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOff")]
    private static extern bool SetNoteOffNative(IntPtr instancePtr, Int64 instrumentId, double timestamp,
                                                float notePitch);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOnCallback")]
    private static extern void SetNoteOnCallbackNative(IntPtr instancePtr, IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOn")]
    private static extern bool SetNoteOnNative(IntPtr instancePtr, Int64 instrumentId, double timestamp,
                                               float notePitch, float noteIntensity);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParam")]
    private static extern bool SetParamNative(IntPtr instancePtr, Int64 instrumentId, double timestamp, int paramId,
                                              float paramValue);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParamToDefault")]
    private static extern bool SetParamToDefaultNative(IntPtr instancePtr, Int64 instrumentId, double timestamp,
                                                       int paramId);

    [DllImport(pluginName, EntryPoint = "BarelySetSampleRate")]
    private static extern void SetSampleRateNative(IntPtr instancePtr, double timestamp, int sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyUpdate")]
    private static extern void UpdateNative(IntPtr instancePtr, double timestamp);
  }
}
