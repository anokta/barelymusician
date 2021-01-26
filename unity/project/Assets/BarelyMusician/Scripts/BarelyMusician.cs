using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public static class BarelyMusician {
    // Invalid id.
    public const int InvalidId = -1;

    // Beat event.
    public delegate void BeatEvent(double dspTime, int beat);
    public static event BeatEvent OnBeat;

    // Note off event.
    public delegate void NoteOffEvent(double dspTime, Instrument instrument, float pitch);
    public static event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(double dspTime, Instrument instrument, float pitch,
                                     float intensity);
    public static event NoteOnEvent OnNoteOn;

    // Internal Unity instrument functions.
    public delegate void UnityProcessFn(
      [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output, int size,
      int numChannels);
    public delegate void UnitySetNoteOffFn(float pitch);
    public delegate void UnitySetNoteOnFn(float pitch, float intensity);
    public delegate void UnitySetParamFn(int id, float value);

    // Creates new instrument.
    public static int Create(Instrument instrument) {
      int id = InvalidId;
      if (BarelyMusicianInternal.Initialized) {
        Type instrumentType = instrument.GetType();
        if (instrumentType.IsSubclassOf(typeof(UnityInstrument))) {
          var unityInstrument = instrument as UnityInstrument;
          id = CreateUnityInstrumentNative(
              Marshal.GetFunctionPointerForDelegate(unityInstrument.ProcessFn),
              Marshal.GetFunctionPointerForDelegate(unityInstrument.SetNoteOffFn),
              Marshal.GetFunctionPointerForDelegate(unityInstrument.SetNoteOnFn),
              Marshal.GetFunctionPointerForDelegate(unityInstrument.SetParamFn));
        } else if (instrumentType == typeof(SynthInstrument)) {
          id = CreateSynthInstrumentNative();
        } else {
          Debug.LogError("Unsupported instrument type: " + instrumentType);
        }
        if (id != InvalidId) {
          BarelyMusicianInternal.Instruments.Add(id, instrument);
        }
      }
      return id;
    }

    // Destroys instrument.
    public static void Destroy(int id) {
      if (BarelyMusicianInternal.Initialized) {
        DestroyNative(id);
        BarelyMusicianInternal.Instruments.Remove(id);
      }
    }

    // Returns playback position.
    public static double GetPosition() {
      if (BarelyMusicianInternal.Initialized) {
        return GetPositionNative();
      }
      return 0.0;
    }

    // Returns playback tempo.
    public static double GetTempo() {
      if (BarelyMusicianInternal.Initialized) {
        return GetTempoNative();
      }
      return 0.0;
    }

    // Returns the playback state.
    public static bool IsPlaying() {
      if (BarelyMusicianInternal.Initialized) {
        return IsPlayingNative();
      }
      return false;
    }

    // Processes instrument.
    public static void Process(int id, float[] output, int numChannels) {
      if (BarelyMusicianInternal.Initialized) {
        ProcessNative(id, AudioSettings.dspTime, output, numChannels, output.Length / numChannels);
      }
    }

    // Schedules instrument note.
    public static void ScheduleNote(int id, double position, double duration, float pitch,
                                    float intensity) {
      if (BarelyMusicianInternal.Initialized) {
        ScheduleNoteNative(id, position, duration, pitch, intensity);
      }
    }

    // Sets all instrument notes off.
    public static void SetAllNotesOff(int id) {
      if (BarelyMusicianInternal.Initialized) {
        SetAllNotesOffNative(id);
      }
    }

    // Sets instrument note off.
    public static void SetNoteOff(int id, float pitch) {
      if (BarelyMusicianInternal.Initialized) {
        SetNoteOffNative(id, pitch);
      }
    }

    // Sets instrument note on.
    public static void SetNoteOn(int id, float pitch, float intensity) {
      if (BarelyMusicianInternal.Initialized) {
        SetNoteOnNative(id, pitch, intensity);
      }
    }

    // Sets instrument param value.
    public static void SetParam(int id, int paramId, float value) {
      if (BarelyMusicianInternal.Initialized) {
        SetParamNative(id, paramId, value);
      }
    }

    // Sets playback position.
    public static void SetPosition(double position) {
      if (BarelyMusicianInternal.Initialized) {
        SetPositionNative(position);
      }
    }

    // Sets playback tempo.
    public static void SetTempo(double tempo) {
      if (BarelyMusicianInternal.Initialized) {
        SetTempoNative(tempo);
      }
    }

    // Starts playback.
    public static void Play() {
      if (BarelyMusicianInternal.Initialized) {
        StartNative(AudioSettings.dspTime + BarelyMusicianInternal.Lookahead);
      }
    }

    // Starts playback at scheduled time.
    public static void PlayScheduled(double dspTime) {
      if (BarelyMusicianInternal.Initialized) {
        StartNative(dspTime);
      }
    }

    // Pauses playback.
    public static void Pause() {
      if (BarelyMusicianInternal.Initialized) {
        PauseNative();
      }
    }

    // Stops playback.
    public static void Stop() {
      if (BarelyMusicianInternal.Initialized) {
        StopNative();
      }
    }

    // Internal component to update the native state.
    private class BarelyMusicianInternal : MonoBehaviour {
      // Denotes whether the internal system is initialized. If not, attempts to initialize before.
      public static bool Initialized {
        get {
          if (_isShuttingDown) {
            return false;
          }
          if (_instance == null) {
            var gameObject = new GameObject() {
              hideFlags = HideFlags.HideAndDontSave
            };
            _instance = gameObject.AddComponent<BarelyMusicianInternal>();
            GameObject.DontDestroyOnLoad(gameObject);
          }
          return _instance != null;
        }
      }

      // List of instruments.
      public static Dictionary<int, Instrument> Instruments { get; private set; }

      // Lookahead in seconds.
      public static double Lookahead { get; private set; }

      // Singleton instance.
      private static BarelyMusicianInternal _instance = null;

      // Denotes whether the component is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Log severity.
      private enum Severity {
        Info = 0,
        Warning = 1,
        Error = 2,
        Fatal = 3,
      }

      // Beat callback.
      private delegate void BeatCallback(double dspTime, int beat);
      private BeatCallback _beatCallback = null;

      // Debug callback.
      private delegate void DebugCallback(int severity, string message);
      private DebugCallback _debugCallback = null;

      // Note off callback.
      private delegate void NoteOffCallback(double dspTime, int id, float pitch);
      private NoteOffCallback _noteOffCallback = null;

      // Note on callback.
      private delegate void NoteOnCallback(double dspTime, int id, float pitch, float intensity);
      private NoteOnCallback _noteOnCallback = null;

      private void Awake() {
        var config = AudioSettings.GetConfiguration();
        InitializeNative(config.sampleRate);
        _beatCallback = delegate (double dspTime, int beat) {
          OnBeat?.Invoke(dspTime, beat);
        };
        SetBeatCallbackNative(Marshal.GetFunctionPointerForDelegate(_beatCallback));
        _debugCallback = delegate (int severity, string message) {
          message = "{::" + pluginName + "::}" + message;
          switch ((Severity)severity) {
            case Severity.Info:
              Debug.Log(message);
              break;
            case Severity.Warning:
              Debug.LogWarning(message);
              break;
            case Severity.Error:
              Debug.LogError(message);
              break;
            case Severity.Fatal:
              Debug.LogAssertion(message);
              break;
          }
        };
        SetDebugCallbackNative(Marshal.GetFunctionPointerForDelegate(_debugCallback));
        _noteOffCallback = delegate (double dspTime, int id, float pitch) {
          Instrument instrument = null;
          if (Instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(dspTime, instrument, pitch);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (double dspTime, int id, float pitch, float intensity) {
          Instrument instrument = null;
          if (Instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(dspTime, instrument, pitch, intensity);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
        Instruments = new Dictionary<int, Instrument>();
        Lookahead = config.dspBufferSize / config.sampleRate + (double)Time.fixedDeltaTime;
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        ShutdownNative();

        GameObject.DestroyImmediate(gameObject);
      }

      private void FixedUpdate() {
        UpdateNative(AudioSettings.dspTime + Lookahead);
      }
    }

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "Initialize")]
    private static extern void InitializeNative(int sampleRate);

    [DllImport(pluginName, EntryPoint = "Shutdown")]
    private static extern void ShutdownNative();

    [DllImport(pluginName, EntryPoint = "CreateUnityInstrument")]
    private static extern int CreateUnityInstrumentNative(
      IntPtr processFnPtr, IntPtr setNoteOffFnPtr, IntPtr setNoteOnFnPtr, IntPtr setParamFnPtr);

    [DllImport(pluginName, EntryPoint = "CreateSynthInstrument")]
    private static extern int CreateSynthInstrumentNative();

    [DllImport(pluginName, EntryPoint = "Destroy")]
    private static extern void DestroyNative(int id);

    [DllImport(pluginName, EntryPoint = "GetParam")]
    private static extern float GetParamNative(int id, int param_id);

    [DllImport(pluginName, EntryPoint = "GetPosition")]
    private static extern double GetPositionNative();

    [DllImport(pluginName, EntryPoint = "GetTempo")]
    private static extern double GetTempoNative();

    [DllImport(pluginName, EntryPoint = "IsNoteOn")]
    private static extern bool IsNoteOnNative(int id, float pitch);

    [DllImport(pluginName, EntryPoint = "IsPlaying")]
    private static extern bool IsPlayingNative();

    [DllImport(pluginName, EntryPoint = "Process")]
    private static extern void ProcessNative(int id, double timestamp, [In, Out] float[] output,
                                             int numChannels, int numFrames);

    [DllImport(pluginName, EntryPoint = "ResetAllParams")]
    private static extern void ResetAllParams(int id);

    [DllImport(pluginName, EntryPoint = "ScheduleNote")]
    private static extern void ScheduleNoteNative(int id, double position, double duration,
                                                  float pitch, float intensity);

    [DllImport(pluginName, EntryPoint = "SetBeatCallback")]
    private static extern void SetBeatCallbackNative(IntPtr beatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetDebugCallback")]
    private static extern void SetDebugCallbackNative(IntPtr debugCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOffCallback")]
    private static extern void SetNoteOffCallbackNative(IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOnCallback")]
    private static extern void SetNoteOnCallbackNative(IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetAllNotesOff")]
    private static extern void SetAllNotesOffNative(int id);

    [DllImport(pluginName, EntryPoint = "SetNoteOff")]
    private static extern void SetNoteOffNative(int id, float pitch);

    [DllImport(pluginName, EntryPoint = "SetNoteOn")]
    private static extern void SetNoteOnNative(int id, float pitch, float intensity);

    [DllImport(pluginName, EntryPoint = "SetParam")]
    private static extern void SetParamNative(int id, int param_id, float value);

    [DllImport(pluginName, EntryPoint = "SetPosition")]
    private static extern void SetPositionNative(double position);

    [DllImport(pluginName, EntryPoint = "SetTempo")]
    private static extern void SetTempoNative(double tempo);

    [DllImport(pluginName, EntryPoint = "Start")]
    private static extern void StartNative(double timestamp);

    [DllImport(pluginName, EntryPoint = "Pause")]
    private static extern void PauseNative();

    [DllImport(pluginName, EntryPoint = "Stop")]
    private static extern void StopNative();

    [DllImport(pluginName, EntryPoint = "Update")]
    private static extern void UpdateNative(double timestamp);
  }
}
