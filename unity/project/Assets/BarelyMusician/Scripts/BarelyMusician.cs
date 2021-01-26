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
      Type instrumentType = instrument.GetType();
      if (instrumentType.IsSubclassOf(typeof(UnityInstrument))) {
        var unityInstrument = instrument as UnityInstrument;
        id = CreateUnityInstrument(
            InstancePtr, Marshal.GetFunctionPointerForDelegate(unityInstrument.ProcessFn),
            Marshal.GetFunctionPointerForDelegate(unityInstrument.SetNoteOffFn),
            Marshal.GetFunctionPointerForDelegate(unityInstrument.SetNoteOnFn),
            Marshal.GetFunctionPointerForDelegate(unityInstrument.SetParamFn));
      } else if (instrumentType == typeof(SynthInstrument)) {
        id = CreateSynthInstrument(InstancePtr);
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      if (id != InvalidId) {
        _instruments.Add(id, instrument);
      }
      return id;
    }

    // Destroys instrument.
    public static void Destroy(int id) {
      Destroy(InstancePtr, id);
      _instruments.Remove(id);
    }

    // Returns playback position.
    public static double GetPosition() {
      return GetPosition(InstancePtr);
    }

    // Returns playback tempo.
    public static double GetTempo() {
      return GetTempo(InstancePtr);
    }

    // Returns the playback state.
    public static bool IsPlaying() {
      return IsPlaying(InstancePtr);
    }

    // Processes instrument.
    public static void Process(int id, float[] output, int numChannels) {
      Process(InstancePtr, id, AudioSettings.dspTime, output, numChannels,
                    output.Length / numChannels);
    }

    // Schedules instrument note.
    public static void ScheduleNote(int id, double position, double duration, float pitch,
                                    float intensity) {
      ScheduleNote(InstancePtr, id, position, duration, pitch, intensity);
    }

    // Sets all instrument notes off.
    public static void SetAllNotesOff(int id) {
      SetAllNotesOff(InstancePtr, id);
    }

    // Sets instrument note off.
    public static void SetNoteOff(int id, float pitch) {
      SetNoteOff(InstancePtr, id, pitch);
    }

    // Sets instrument note on.
    public static void SetNoteOn(int id, float pitch, float intensity) {
      SetNoteOn(InstancePtr, id, pitch, intensity);
    }

    // Sets instrument param value.
    public static void SetParam(int id, int paramId, float value) {
      SetParam(InstancePtr, id, paramId, value);
    }

    // Sets playback position.
    public static void SetPosition(double position) {
      SetPosition(InstancePtr, position);
    }

    // Sets playback tempo.
    public static void SetTempo(double tempo) {
      SetTempo(InstancePtr, tempo);
    }

    // Starts playback.
    public static void Play() {
      Start(InstancePtr, AudioSettings.dspTime + _lookahead);
    }

    // Starts playback at scheduled time.
    public static void PlayScheduled(double dspTime) {
      Start(InstancePtr, dspTime);
    }

    // Pauses playback.
    public static void Pause() {
      Pause(InstancePtr);
    }

    // Stops playback.
    public static void Stop() {
      Stop(InstancePtr);
    }

    // Singleton instance.
    private static IntPtr InstancePtr {
      get {
        if (_isShuttingDown) {
          return IntPtr.Zero;
        }
        if (_instancePtr == IntPtr.Zero) {
          var gameObject = new GameObject() {
            hideFlags = HideFlags.HideAndDontSave
          }.AddComponent<BarelyMusicianInternal>();
          GameObject.DontDestroyOnLoad(gameObject);
        }
        return _instancePtr;
      }
    }
    private static IntPtr _instancePtr = IntPtr.Zero;

    // Denotes whether the system is shutting down to avoid re-initialization.
    private static bool _isShuttingDown = false;

    // List of instruments.
    private static Dictionary<int, Instrument> _instruments = new Dictionary<int, Instrument>();

    // Lookahead in seconds.
    private static double _lookahead = 0.0;

    // Internal component to update the native state.
    private class BarelyMusicianInternal : MonoBehaviour {
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
        _instancePtr = Initialize(config.sampleRate);
        _beatCallback = delegate (double dspTime, int beat) {
          OnBeat?.Invoke(dspTime, beat);
        };
        SetBeatCallback(_instancePtr, Marshal.GetFunctionPointerForDelegate(_beatCallback));
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
        SetDebugCallback(_instancePtr, Marshal.GetFunctionPointerForDelegate(_debugCallback));
        _noteOffCallback = delegate (double dspTime, int id, float pitch) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(dspTime, instrument, pitch);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallback(_instancePtr, Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (double dspTime, int id, float pitch, float intensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(dspTime, instrument, pitch, intensity);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallback(_instancePtr, Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
        _lookahead = config.dspBufferSize / config.sampleRate + (double)Time.fixedDeltaTime;
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        Shutdown(_instancePtr);

        GameObject.DestroyImmediate(gameObject);
      }

      private void FixedUpdate() {
        Update(_instancePtr, AudioSettings.dspTime + _lookahead);
      }
    }

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "Initialize")]
    private static extern IntPtr Initialize(int sampleRate);

    [DllImport(pluginName, EntryPoint = "Shutdown")]
    private static extern void Shutdown(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "CreateUnityInstrument")]
    private static extern int CreateUnityInstrument(IntPtr instancePtr, IntPtr processFnPtr,
                                                    IntPtr setNoteOffFnPtr, IntPtr setNoteOnFnPtr,
                                                    IntPtr setParamFnPtr);

    [DllImport(pluginName, EntryPoint = "CreateSynthInstrument")]
    private static extern int CreateSynthInstrument(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "Destroy")]
    private static extern void Destroy(IntPtr instancePtr, int id);

    [DllImport(pluginName, EntryPoint = "GetParam")]
    private static extern float GetParam(IntPtr instancePtr, int id, int param_id);

    [DllImport(pluginName, EntryPoint = "GetPosition")]
    private static extern double GetPosition(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "GetTempo")]
    private static extern double GetTempo(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "IsNoteOn")]
    private static extern bool IsNoteOn(IntPtr instancePtr, int id, float pitch);

    [DllImport(pluginName, EntryPoint = "IsPlaying")]
    private static extern bool IsPlaying(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "Process")]
    private static extern void Process(IntPtr instancePtr, int id, double timestamp,
                                       [In, Out] float[] output, int numChannels, int numFrames);

    [DllImport(pluginName, EntryPoint = "ResetAllParams")]
    private static extern void ResetAllParams(IntPtr instancePtr, int id);

    [DllImport(pluginName, EntryPoint = "ScheduleNote")]
    private static extern void ScheduleNote(IntPtr instancePtr, int id, double position,
                                            double duration, float pitch, float intensity);

    [DllImport(pluginName, EntryPoint = "SetBeatCallback")]
    private static extern void SetBeatCallback(IntPtr instancePtr, IntPtr beatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetDebugCallback")]
    private static extern void SetDebugCallback(IntPtr instancePtr, IntPtr debugCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOffCallback")]
    private static extern void SetNoteOffCallback(IntPtr instancePtr, IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOnCallback")]
    private static extern void SetNoteOnCallback(IntPtr instancePtr, IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetAllNotesOff")]
    private static extern void SetAllNotesOff(IntPtr instancePtr, int id);

    [DllImport(pluginName, EntryPoint = "SetNoteOff")]
    private static extern void SetNoteOff(IntPtr instancePtr, int id, float pitch);

    [DllImport(pluginName, EntryPoint = "SetNoteOn")]
    private static extern void SetNoteOn(IntPtr instancePtr, int id, float pitch, float intensity);

    [DllImport(pluginName, EntryPoint = "SetParam")]
    private static extern void SetParam(IntPtr instancePtr, int id, int param_id, float value);

    [DllImport(pluginName, EntryPoint = "SetPosition")]
    private static extern void SetPosition(IntPtr instancePtr, double position);

    [DllImport(pluginName, EntryPoint = "SetTempo")]
    private static extern void SetTempo(IntPtr instancePtr, double tempo);

    [DllImport(pluginName, EntryPoint = "Start")]
    private static extern void Start(IntPtr instancePtr, double timestamp);

    [DllImport(pluginName, EntryPoint = "Pause")]
    private static extern void Pause(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "Stop")]
    private static extern void Stop(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "Update")]
    private static extern void Update(IntPtr instancePtr, double timestamp);
  }
}
