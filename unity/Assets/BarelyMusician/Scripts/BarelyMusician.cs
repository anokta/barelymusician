﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public static class BarelyMusician {
    // Invalid id.
    public const Int64 InvalidId = -1;

    // Beat event.
    public delegate void BeatEvent(double dspTime, int beat);
    public static event BeatEvent OnBeat;

    // Note off event.
    public delegate void NoteOffEvent(double dspTime, Instrument instrument, float noteIndex);
    public static event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(double dspTime, Instrument instrument, float noteIndex,
                                     float noteIntenstiy);
    public static event NoteOnEvent OnNoteOn;

    // Internal Unity instrument functions.
    public delegate void UnityNoteOffFn(float index);
    public delegate void UnityNoteOnFn(float index, float intensity);
    public delegate void UnityProcessFn(
      [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output, int size,
      int numChannels);

    // Creates new instrument.
    public static Int64 Create(Instrument instrument) {
      return BarelyMusicianInternal.Instance?.Create(instrument) ?? InvalidId;
    }

    // Destroys instrument.
    public static void Destroy(Int64 id) {
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
    public static void NoteOff(Int64 id, float index) {
      BarelyMusicianInternal.Instance?.NoteOff(id, index);
    }

    // Sets instrument note on.
    public static void NoteOn(Int64 id, float index, float intensity) {
      BarelyMusicianInternal.Instance?.NoteOn(id, index, intensity);
    }

    // Processes instrument.
    public static void Process(Int64 id, float[] output, int numChannels) {
      BarelyMusicianInternal.Instance?.Process(id, output, numChannels);
    }

    // Schedules instrument note.
    public static void ScheduleNote(Int64 id, double position, double duration, float index,
                                    float intensity) {
      BarelyMusicianInternal.Instance?.ScheduleNote(id, position, duration, index, intensity);
    }

    // Schedules instrument note off.
    public static void ScheduleNoteOff(Int64 id, double position, float index) {
      BarelyMusicianInternal.Instance?.ScheduleNoteOff(id, position, index);
    }

    // Schedules instrument note on.
    public static void ScheduleNoteOn(Int64 id, double position, float index, float intensity) {
      BarelyMusicianInternal.Instance?.ScheduleNoteOn(id, position, index, intensity);
    }

    // Sets instrument param value.
    public static void SetParam(Int64 id, int paramId, float value) {
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
      private delegate void NoteOffCallback(double dspTime, Int64 id, float index);
      private NoteOffCallback _noteOffCallback = null;

      // Note on callback.
      private delegate void NoteOnCallback(double dspTime, Int64 id, float index, float intensity);
      private NoteOnCallback _noteOnCallback = null;

      // List of instruments.
      private static Dictionary<Int64, Instrument> _instruments;

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
        _noteOffCallback = delegate (double dspTime, Int64 id, float index) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(dspTime, instrument, index);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (double dspTime, Int64 id, float index, float intensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(dspTime, instrument, index, intensity);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallbackNative(Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
        _instruments = new Dictionary<Int64, Instrument>();
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

      public Int64 Create(Instrument instrument) {
        Int64 id = InvalidId;
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

      public void Destroy(Int64 id) {
        _instruments.Remove(id);
        DestroyNative(id);
      }

      public double GetPosition() { return GetPositionNative(); }

      public double GetTempo() { return GetTempoNative(); }

      public bool IsPlaying() { return IsPlayingNative(); }

      public void NoteOff(Int64 id, float index) {
        NoteOffNative(id, index);
      }

      public void NoteOn(Int64 id, float index, float intensity) {
        NoteOnNative(id, index, intensity);
      }

      public void Process(Int64 id, float[] output, int numChannels) {
        int numFrames = output.Length / numChannels;
        ProcessNative(id, AudioSettings.dspTime, output, numChannels, numFrames);
      }

      public void ScheduleNote(Int64 id, double position, double duration, float index,
                                      float intensity) {
        ScheduleNoteNative(id, position, duration, index, intensity);
      }

      public void ScheduleNoteOff(Int64 id, double position, float index) {
        ScheduleNoteOffNative(id, position, index);
      }

      public void ScheduleNoteOn(Int64 id, double position, float index, float intensity) {
        ScheduleNoteOnNative(id, position, index, intensity);
      }

      public void SetParam(Int64 id, int paramId, float value) {
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
    private static extern Int64 CreateUnityInstrumentNative(IntPtr noteOffFnPtr, IntPtr noteOnFnPtr,
                                                            IntPtr processFnPtr);

    [DllImport(pluginName, EntryPoint = "CreateBasicSynthInstrument")]
    private static extern Int64 CreateBasicSynthInstrumentNative();

    [DllImport(pluginName, EntryPoint = "Destroy")]
    private static extern void DestroyNative(Int64 id);

    [DllImport(pluginName, EntryPoint = "GetParam")]
    private static extern float GetParamNative(Int64 id, int param_id);

    [DllImport(pluginName, EntryPoint = "GetPosition")]
    private static extern double GetPositionNative();

    [DllImport(pluginName, EntryPoint = "GetTempo")]
    private static extern double GetTempoNative();

    [DllImport(pluginName, EntryPoint = "IsNoteOn")]
    private static extern bool IsNoteOnNative(Int64 id, float index);

    [DllImport(pluginName, EntryPoint = "IsPlaying")]
    private static extern bool IsPlayingNative();

    [DllImport(pluginName, EntryPoint = "Process")]
    private static extern void ProcessNative(Int64 id, double timestamp, [In, Out] float[] output,
                                             int numChannels, int numFrames);

    [DllImport(pluginName, EntryPoint = "AllNotesOff")]
    private static extern void AllNotesOffNative(Int64 id);

    [DllImport(pluginName, EntryPoint = "NoteOff")]
    private static extern void NoteOffNative(Int64 id, float index);

    [DllImport(pluginName, EntryPoint = "NoteOn")]
    private static extern void NoteOnNative(Int64 id, float index, float intensity);

    [DllImport(pluginName, EntryPoint = "ResetAllParams")]
    private static extern void ResetAllParams(Int64 id);

    [DllImport(pluginName, EntryPoint = "ScheduleNote")]
    private static extern void ScheduleNoteNative(Int64 id, double position, double duration,
                                                  float index, float intensity);

    [DllImport(pluginName, EntryPoint = "ScheduleNoteOff")]
    private static extern void ScheduleNoteOffNative(Int64 id, double position, float index);

    [DllImport(pluginName, EntryPoint = "ScheduleNoteOn")]
    private static extern void ScheduleNoteOnNative(Int64 id, double position, float index,
                                                    float intensity);

    [DllImport(pluginName, EntryPoint = "SetBeatCallback")]
    private static extern void SetBeatCallbackNative(IntPtr beatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetDebugCallback")]
    private static extern void SetDebugCallbackNative(IntPtr debugCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOffCallback")]
    private static extern void SetNoteOffCallbackNative(IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetNoteOnCallback")]
    private static extern void SetNoteOnCallbackNative(IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "SetParam")]
    private static extern void SetParamNative(Int64 id, int param_id, float value);

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
