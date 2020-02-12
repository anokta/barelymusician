using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  [RequireComponent(typeof(AudioListener))]
  public class BarelyMusician : MonoBehaviour {
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

    // Invalid ID.
    public const int InvalidId = -1;

    // Singleton instance.
    public static BarelyMusician Instance {
      get {
        if (_instance == null) {
          var existingInstance = GameObject.FindObjectOfType<BarelyMusician>();
          if (existingInstance != null) {
            _instance = existingInstance;
          } else {
            var audioListener = GameObject.FindObjectOfType<AudioListener>();
            if (audioListener != null) {
              _instance = audioListener.gameObject.AddComponent<BarelyMusician>();
            } else {
              Debug.LogError("No AudioListener found in the scene!");
            }
          }
        }
        return _instance;
      }
    }
    private static BarelyMusician _instance = null;

    // Internal event callbacks.
    private delegate void BeatCallback(int beat);
    private delegate void NoteOffCallback(int id, float index);
    private delegate void NoteOnCallback(int id, float index, float intensity);

    // List of instruments.
    private Dictionary<int, Instrument> _instruments = null;

    // Internal beat callback.
    private BeatCallback _beatCallback = null;

    // Internal note off callback.
    private NoteOffCallback _noteOffCallback = null;

    // Internal note on callback.
    private NoteOnCallback _noteOnCallback = null;

    // Audio thread last DSP time.
    private double _lastDspTime = 0.0;

    private void Awake() {
      _instruments = new Dictionary<int, Instrument>();
      _beatCallback = delegate (int beat) {
        OnBeat?.Invoke(beat);
      };
      _noteOffCallback = delegate (int id, float index) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(id, out instrument)) {
          OnNoteOff?.Invoke(instrument, index);
        } else {
          Debug.LogError("Instrument does not exist: " + id);
        }
      };
      _noteOnCallback = delegate (int id, float index, float intensity) {
        Instrument instrument = null;
        if (_instruments.TryGetValue(id, out instrument)) {
          OnNoteOn?.Invoke(instrument, index, intensity);
        } else {
          Debug.LogError("Instrument does not exist: " + id);
        }
      };
      InitializeNative(AudioSettings.outputSampleRate);
      SetBeatCallbackNative(Marshal.GetFunctionPointerForDelegate(_beatCallback));
      SetNoteOffCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
      SetNoteOnCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
    }

    private void OnDestroy() {
      ShutdownNative();
      _beatCallback = null;
      _noteOffCallback = null;
      _noteOnCallback = null;
      _instruments = null;
      _instance = null;
    }

    private void Update() {
      UpdateMainThreadNative();
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      UpdateAudioThread(data.Length / channels);
    }

    // Creates new instrument.
    public int Create(Instrument instrument) {
      int id = InvalidId;
      var instrumentType = instrument.GetType();
      if (instrumentType.IsSubclassOf(typeof(UnityInstrument))) {
        var unityInstrument = instrument as UnityInstrument;
        id = CreateNative(Marshal.GetFunctionPointerForDelegate(unityInstrument.NoteOffFn),
                          Marshal.GetFunctionPointerForDelegate(unityInstrument.NoteOnFn),
                          Marshal.GetFunctionPointerForDelegate(unityInstrument.ProcessFn));
        _instruments.Add(id, instrument);
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      return id;
    }

    // Destroys instrument.
    public void Destroy(int id) {
      _instruments.Remove(id);
      DestroyNative(id);
    }

    // Returns playback position.
    public double GetPosition() {
      return GetPositionNative();
    }

    // Returns playback tempo.
    public double GetTempo() {
      return GetTempoNative();
    }

    // Returns the playback state.
    public bool IsPlaying() {
      return IsPlayingNative();
    }

    // Processes instrument.
    public void Process(int id, float[] output, int numChannels) {
      int numFrames = output.Length / numChannels;
      UpdateAudioThread(numFrames);
      ProcessNative(id, output, numChannels, numFrames);
    }

    // Sets instrument note off.
    public void NoteOff(int id, float index) {
      NoteOffNative(id, index);
    }

    // Sets instrument note on.
    public void NoteOn(int id, float index, float intensity) {
      NoteOnNative(id, index, intensity);
    }

    // Schedules instrument note off.
    public void ScheduleNoteOff(int id, double position, float index) {
      ScheduleNoteOffNative(id, position, index);
    }

    // Schedules instrument note on.
    public void ScheduleNoteOn(int id, double position, float index, float intensity) {
      ScheduleNoteOnNative(id, position, index, intensity);
    }

    // Sets playback position.
    public void SetPosition(double position) {
      SetPositionNative(position);
    }

    // Sets playback tempo.
    public void SetTempo(double tempo) {
      SetTempoNative(tempo);
    }

    // Starts playback.
    public void Play() {
      StartNative();
    }

    // Pauses playback.
    public void Pause() {
      StopNative();
    }

    // Stops playback.
    public void Stop() {
      StopNative();
      SetPosition(0.0);
    }

    // Updates the audio thread state.
    private void UpdateAudioThread(int numFrames) {
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
