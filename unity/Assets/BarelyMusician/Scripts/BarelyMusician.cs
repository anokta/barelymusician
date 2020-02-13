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
    public delegate void BeatEvent(int beat);
    public static event BeatEvent OnBeat;

    // Note off event.
    public delegate void NoteOffEvent(Instrument instrument, float index);
    public static event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(Instrument instrument, float index, float intenstiy);
    public static event NoteOnEvent OnNoteOn;

    // Internal Unity instrument functions.
    public delegate void UnityNoteOffFn(float index);
    public delegate void UnityNoteOnFn(float index, float intensity);
    public delegate void UnityProcessFn(
      [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output, int size,
      int numChannels);

    // Creates new instrument.
    public static int Create(Instrument instrument) {
      int id = InvalidId;
      if (BarelyMusicianInternal.Instance != null) {
        Type instrumentType = instrument.GetType();
        if (instrumentType.IsSubclassOf(typeof(UnityInstrument))) {
          var unityInstrument = instrument as UnityInstrument;
          id = CreateNative(Marshal.GetFunctionPointerForDelegate(unityInstrument.NoteOffFn),
                            Marshal.GetFunctionPointerForDelegate(unityInstrument.NoteOnFn),
                            Marshal.GetFunctionPointerForDelegate(unityInstrument.ProcessFn));
          _instruments.Add(id, instrument);
        } else {
          Debug.LogError("Unsupported instrument type: " + instrumentType);
        }
      }
      return id;
    }

    // Destroys instrument.
    public static void Destroy(int id) {
      if (BarelyMusicianInternal.Instance != null) {
        _instruments.Remove(id);
        DestroyNative(id);
      }
    }

    // Returns playback position.
    public static double GetPosition() {
      if (BarelyMusicianInternal.Instance != null) {
        return GetPositionNative();
      }
      return 0.0;
    }

    // Returns playback tempo.
    public static double GetTempo() {
      if (BarelyMusicianInternal.Instance != null) {
        return GetTempoNative();
      }
      return 0.0;
    }

    // Returns the playback state.
    public static bool IsPlaying() {
      if (BarelyMusicianInternal.Instance != null) {
        return IsPlayingNative();
      }
      return false;
    }

    // Sets instrument note off.
    public static void NoteOff(int id, float index) {
      if (BarelyMusicianInternal.Instance != null) {
        NoteOffNative(id, index);
      }
    }

    // Sets instrument note on.
    public static void NoteOn(int id, float index, float intensity) {
      if (BarelyMusicianInternal.Instance != null) {
        NoteOnNative(id, index, intensity);
      }
    }

    // Processes instrument.
    public static void Process(int id, float[] output, int numChannels) {
      int numFrames = output.Length / numChannels;
      UpdateAudioThread(numFrames);
      ProcessNative(id, output, numChannels, numFrames);
    }

    // Schedules instrument note off.
    public static void ScheduleNoteOff(int id, double position, float index) {
      if (BarelyMusicianInternal.Instance != null) {
        ScheduleNoteOffNative(id, position, index);
      }
    }

    // Schedules instrument note on.
    public static void ScheduleNoteOn(int id, double position, float index, float intensity) {
      if (BarelyMusicianInternal.Instance != null) {
        ScheduleNoteOnNative(id, position, index, intensity);
      }
    }

    // Sets playback position.
    public static void SetPosition(double position) {
      if (BarelyMusicianInternal.Instance != null) {
        SetPositionNative(position);
      }
    }

    // Sets playback tempo.
    public static void SetTempo(double tempo) {
      if (BarelyMusicianInternal.Instance != null) {
        SetTempoNative(tempo);
      }
    }

    // Starts playback.
    public static void Play() {
      if (BarelyMusicianInternal.Instance != null) {
        StartNative();
      }
    }

    // Pauses playback.
    public static void Pause() {
      if (BarelyMusicianInternal.Instance != null) {
        StopNative();
      }
    }

    // Stops playback.
    public static void Stop() {
      if (BarelyMusicianInternal.Instance != null) {
        StopNative();
        SetPosition(0.0);
      }
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

      // Beat callback.
      private delegate void BeatCallback(int beat);
      private BeatCallback _beatCallback = null;

      // Note off callback.
      private delegate void NoteOffCallback(int id, float index);
      private NoteOffCallback _noteOffCallback = null;

      // Note on callback.
      private delegate void NoteOnCallback(int id, float index, float intensity);
      private NoteOnCallback _noteOnCallback = null;

      // Audio source.
      private AudioSource _source;

      private void Awake() {
        InitializeNative(AudioSettings.outputSampleRate);
        _beatCallback = delegate (int beat) {
          OnBeat?.Invoke(beat);
        };
        SetBeatCallbackNative(Marshal.GetFunctionPointerForDelegate(_beatCallback));
        _noteOffCallback = delegate (int id, float index) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(instrument, index);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (int id, float index, float intensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(instrument, index, intensity);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOnCallback));

        _source = gameObject.AddComponent<AudioSource>();
        _source.bypassEffects = true;
        _source.bypassListenerEffects = true;
        _source.bypassReverbZones = true;
        _source.dopplerLevel = 0.0f;
        _source.playOnAwake = false;
        _source.priority = 0;
        _source.spatialBlend = 0.0f;
      }

      private void OnEnable() {
        _source.Play();
      }

      private void OnDisable() {
        _source.Stop();
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        ShutdownNative();

        GameObject.DestroyImmediate(gameObject);
      }

      private void Update() {
        UpdateMainThreadNative();
      }

      private void OnAudioFilterRead(float[] data, int channels) {
        UpdateAudioThread(data.Length / channels);
      }
    }

    // List of instruments.
    public static Dictionary<int, Instrument> _instruments = new Dictionary<int, Instrument>();

    // Audio thread last DSP time.
    private static double _lastDspTime = 0.0;

    // Updates the audio thread state.
    private static void UpdateAudioThread(int numFrames) {
      double dspTime = AudioSettings.dspTime;
      if (dspTime > _lastDspTime) {
        _lastDspTime = dspTime;
        UpdateAudioThreadNative(numFrames);
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

    [DllImport(pluginName, EntryPoint = "Create")]
    private static extern int CreateNative(IntPtr noteOffFnPtr, IntPtr noteOnFnPtr,
                                           IntPtr processFnPtr);

    [DllImport(pluginName, EntryPoint = "Destroy")]
    private static extern void DestroyNative(int id);

    [DllImport(pluginName, EntryPoint = "GetPosition")]
    private static extern double GetPositionNative();

    [DllImport(pluginName, EntryPoint = "GetTempo")]
    private static extern double GetTempoNative();

    [DllImport(pluginName, EntryPoint = "IsPlaying")]
    private static extern bool IsPlayingNative();

    [DllImport(pluginName, EntryPoint = "Process")]
    private static extern void ProcessNative(int id, [In, Out] float[] output, int numChannels,
                                             int numFrames);

    [DllImport(pluginName, EntryPoint = "NoteOff")]
    private static extern void NoteOffNative(int id, float index);

    [DllImport(pluginName, EntryPoint = "NoteOn")]
    private static extern void NoteOnNative(int id, float index, float intensity);

    [DllImport(pluginName, EntryPoint = "ScheduleNoteOff")]
    private static extern void ScheduleNoteOffNative(int id, double position, float index);

    [DllImport(pluginName, EntryPoint = "ScheduleNoteOn")]
    private static extern void ScheduleNoteOnNative(int id, double position, float index,
                                                    float intensity);

    [DllImport(pluginName, EntryPoint = "SetBeatCallback")]
    private static extern void SetBeatCallbackNative(IntPtr beatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOffCallback")]
    private static extern void SetNoteOffCallbackNative(IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOnCallback")]
    private static extern void SetNoteOnCallbackNative(IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetPosition")]
    private static extern void SetPositionNative(double position);

    [DllImport(pluginName, EntryPoint = "SetTempo")]
    private static extern void SetTempoNative(double tempo);

    [DllImport(pluginName, EntryPoint = "Start")]
    private static extern void StartNative();

    [DllImport(pluginName, EntryPoint = "Stop")]
    private static extern void StopNative();

    [DllImport(pluginName, EntryPoint = "UpdateAudioThread")]
    private static extern void UpdateAudioThreadNative(int numFrames);

    [DllImport(pluginName, EntryPoint = "UpdateMainThread")]
    private static extern void UpdateMainThreadNative();
  }
}
