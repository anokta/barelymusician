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
    public delegate void UnityNoteOffFn(float pitch);
    public delegate void UnityNoteOnFn(float pitch, float intensity);
    public delegate void UnityProcessFn(
      [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output, int size,
      int numChannels);

    // Creates new instrument.
    public static int Create(Instrument instrument) {
      return BarelyMusicianInternal.Instance?.Create(instrument) ?? InvalidId;
    }

    // Destroys instrument.
    public static void Destroy(int id) {
      BarelyMusicianInternal.Instance?.Destroy(id);
    }

    // Returns playback position.
    public static double GetPosition() {
      return BarelyMusicianInternal.Instance?.GetPosition() ?? 0.0;
    }

    // Returns playback tempo.
    public static double GetTempo() {
      return BarelyMusicianInternal.Instance?.GetTempo() ?? 0.0;
    }

    // Returns the playback state.
    public static bool IsPlaying() {
      return BarelyMusicianInternal.Instance?.IsPlaying() ?? false;
    }

    // Sets instrument note off.
    public static void NoteOff(int id, float pitch) {
      BarelyMusicianInternal.Instance?.NoteOff(id, pitch);
    }

    // Sets instrument note on.
    public static void NoteOn(int id, float pitch, float intensity) {
      BarelyMusicianInternal.Instance?.NoteOn(id, pitch, intensity);
    }

    // Processes instrument.
    public static void Process(int id, float[] output, int numChannels) {
      BarelyMusicianInternal.Instance?.Process(id, output, numChannels);
    }

    // Schedules instrument note.
    public static void ScheduleNote(int id, double position, double duration, float pitch,
                                    float intensity) {
      BarelyMusicianInternal.Instance?.ScheduleNote(id, position, duration, pitch, intensity);
    }

    // Sets instrument param value.
    public static void SetParam(int id, int paramId, float value) {
      BarelyMusicianInternal.Instance?.SetParam(id, paramId, value);
    }

    // Sets playback position.
    public static void SetPosition(double position) {
      BarelyMusicianInternal.Instance?.SetPosition(position);
    }

    // Sets playback tempo.
    public static void SetTempo(double tempo) {
      BarelyMusicianInternal.Instance?.SetTempo(tempo);
    }

    // Starts playback.
    public static void Play() {
      BarelyMusicianInternal.Instance?.Play();
    }

    // Starts playback at scheduled time.
    public static void PlayScheduled(double dspTime) {
      BarelyMusicianInternal.Instance?.PlayScheduled(dspTime);
    }

    // Pauses playback.
    public static void Pause() {
      BarelyMusicianInternal.Instance?.Pause();
    }

    // Stops playback.
    public static void Stop() {
      BarelyMusicianInternal.Instance?.Stop();
    }

    // Internal component to update the native state.
    private class BarelyMusicianInternal : MonoBehaviour {
      // Singleton instance.
      public static BarelyMusicianInternal Instance {
        get {
          if (_isShuttingDown) {
            return null;
          }
          if (_instance == null) {
            var gameObject = new GameObject() {
              hideFlags = HideFlags.HideAndDontSave
            };
            _instance = gameObject.AddComponent<BarelyMusicianInternal>();
            GameObject.DontDestroyOnLoad(gameObject);
          }
          return _instance;
        }
      }
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

      // List of instruments.
      private static Dictionary<int, Instrument> _instruments;

      // Lookahead in seconds.
      private double _lookahead = 0.0;

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
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(dspTime, instrument, pitch);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (double dspTime, int id, float pitch, float intensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(dspTime, instrument, pitch, intensity);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
        _instruments = new Dictionary<int, Instrument>();
        _lookahead = config.dspBufferSize / config.sampleRate + (double)Time.fixedDeltaTime;
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        ShutdownNative();

        GameObject.DestroyImmediate(gameObject);
      }

      private void FixedUpdate() {
        UpdateNative(AudioSettings.dspTime + _lookahead);
      }

      public int Create(Instrument instrument) {
        int id = InvalidId;
        Type instrumentType = instrument.GetType();
        if (instrumentType.IsSubclassOf(typeof(UnityInstrument))) {
          var unityInstrument = instrument as UnityInstrument;
          id = CreateUnityInstrumentNative(
              Marshal.GetFunctionPointerForDelegate(unityInstrument.NoteOffFn),
              Marshal.GetFunctionPointerForDelegate(unityInstrument.NoteOnFn),
              Marshal.GetFunctionPointerForDelegate(unityInstrument.ProcessFn));
        } else if (instrumentType == typeof(BasicSynthInstrument)) {
          id = CreateBasicSynthInstrumentNative();
        } else {
          Debug.LogError("Unsupported instrument type: " + instrumentType);
        }
        if (id != InvalidId) {
          _instruments.Add(id, instrument);
        }
        return id;
      }

      public void Destroy(int id) {
        DestroyNative(id);
        _instruments.Remove(id);
      }

      public double GetPosition() { return GetPositionNative(); }

      public double GetTempo() { return GetTempoNative(); }

      public bool IsPlaying() { return IsPlayingNative(); }

      public void NoteOff(int id, float pitch) {
        NoteOffNative(id, pitch);
      }

      public void NoteOn(int id, float pitch, float intensity) {
        NoteOnNative(id, pitch, intensity);
      }

      public void Process(int id, float[] output, int numChannels) {
        int numFrames = output.Length / numChannels;
        ProcessNative(id, AudioSettings.dspTime, output, numChannels, numFrames);
      }

      public void ScheduleNote(int id, double position, double duration, float pitch,
                               float intensity) {
        ScheduleNoteNative(id, position, duration, pitch, intensity);
      }

      public void SetParam(int id, int paramId, float value) {
        SetParamNative(id, paramId, value);
      }

      public void SetPosition(double position) {
        SetPositionNative(position);
      }

      public void SetTempo(double tempo) {
        SetTempoNative(tempo);
      }

      public void Play() {
        PlayScheduled(AudioSettings.dspTime + _lookahead);
      }

      public void PlayScheduled(double dspTime) {
        StartNative(dspTime);
      }

      public void Pause() {
        PauseNative();
      }

      public void Stop() {
        StopNative();
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
    private static extern int CreateUnityInstrumentNative(IntPtr noteOffFnPtr, IntPtr noteOnFnPtr,
                                                          IntPtr processFnPtr);

    [DllImport(pluginName, EntryPoint = "CreateBasicSynthInstrument")]
    private static extern int CreateBasicSynthInstrumentNative();

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

    [DllImport(pluginName, EntryPoint = "AllNotesOff")]
    private static extern void AllNotesOffNative(int id);

    [DllImport(pluginName, EntryPoint = "NoteOff")]
    private static extern void NoteOffNative(int id, float pitch);

    [DllImport(pluginName, EntryPoint = "NoteOn")]
    private static extern void NoteOnNative(int id, float pitch, float intensity);

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
