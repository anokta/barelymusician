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

    // Note off event.
    public delegate void NoteOffEvent(Instrument instrument, float pitch);
    public static event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(Instrument instrument, float pitch, float intensity);
    public static event NoteOnEvent OnNoteOn;

    // Internal Unity instrument functions.
    public delegate void UnityProcessFn([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output,
                                        int size, int numChannels);
    public delegate void UnitySetNoteOffFn(float pitch);
    public delegate void UnitySetNoteOnFn(float pitch, float intensity);
    public delegate void UnitySetParamFn(int id, float value);

    // Creates new instrument.
    public static int Create(Instrument instrument) {
      int id = InvalidId;
      Type instrumentType = instrument.GetType();
      if (instrumentType == typeof(SynthInstrument)) {
        id = CreateSynthInstrumentNative(InstancePtr);
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
      DestroyNative(InstancePtr, id);
      _instruments.Remove(id);
    }

    // Processes instrument.
    public static void Process(int id, float[] output, int numChannels, double dspTime) {
      ProcessNative(InstancePtr, id, output, numChannels, output.Length / numChannels, dspTime);
    }

    // Sets all instrument notes off.
    public static void SetAllNotesOff(int id, double dspTime) {
      SetAllNotesOffNative(InstancePtr, id, dspTime);
    }

    // Sets instrument note off.
    public static void SetNoteOff(int id, float pitch, double dspTime) {
      SetNoteOffNative(InstancePtr, id, pitch, dspTime);
    }

    // Sets instrument note on.
    public static void SetNoteOn(int id, float pitch, float intensity, double dspTime) {
      SetNoteOnNative(InstancePtr, id, pitch, intensity, dspTime);
    }

    // Sets instrument param value.
    public static void SetParam(int id, int paramId, float value, double dspTime) {
      SetParamNative(InstancePtr, id, paramId, value, dspTime);
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

    // Internal component to update the native state.
    private class BarelyMusicianInternal : MonoBehaviour {
      // Log severity.
      private enum Severity {
        Info = 0,
        Warning = 1,
        Error = 2,
        Fatal = 3,
      }

      // Debug callback.
      private delegate void DebugCallback(int severity, string message);
      private DebugCallback _debugCallback = null;

      // Note off callback.
      private delegate void NoteOffCallback(int id, float pitch);
      private NoteOffCallback _noteOffCallback = null;

      // Note on callback.
      private delegate void NoteOnCallback(int id, float pitch, float intensity);
      private NoteOnCallback _noteOnCallback = null;

      private void Awake() {
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
        _instancePtr =
            InitializeNative(AudioSettings.outputSampleRate, Marshal.GetFunctionPointerForDelegate(_debugCallback));
        _noteOffCallback = delegate (int id, float pitch) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOff?.Invoke(instrument, pitch);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOffCallbackNative(_instancePtr, Marshal.GetFunctionPointerForDelegate(_noteOffCallback));
        _noteOnCallback = delegate (int id, float pitch, float intensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(id, out instrument)) {
            OnNoteOn?.Invoke(instrument, pitch, intensity);
          } else {
            Debug.LogError("Instrument does not exist: " + id);
          }
        };
        SetNoteOnCallbackNative(_instancePtr, Marshal.GetFunctionPointerForDelegate(_noteOnCallback));
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        ShutdownNative(_instancePtr);

        GameObject.DestroyImmediate(gameObject);
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
    private static extern int CreateSynthInstrumentNative(IntPtr instancePtr);

    [DllImport(pluginName, EntryPoint = "BarelyDestroyInstrument")]
    private static extern void DestroyNative(IntPtr instancePtr, int instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyGetInstrumentParam")]
    private static extern float GetParamNative(IntPtr instancePtr, int instrumentId, int param_id);

    [DllImport(pluginName, EntryPoint = "BarelyIsInstrumentNoteOn")]
    private static extern bool IsNoteOnNative(IntPtr instancePtr, int instrumentId, float pitch);

    [DllImport(pluginName, EntryPoint = "BarelyProcessInstrument")]
    private static extern void ProcessNative(IntPtr instancePtr, int instrumentId, [In, Out] float[] output,
                                             int numChannels, int numFrames, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyResetAllInstrumentParams")]
    private static extern void ResetAllParamsNative(IntPtr instancePtr, int instrumentId, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentNotesOff")]
    private static extern void SetAllNotesOffNative(IntPtr instancePtr, int instrumentId, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOffCallback")]
    private static extern void SetNoteOffCallbackNative(IntPtr instancePtr, IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOnCallback")]
    private static extern void SetNoteOnCallbackNative(IntPtr instancePtr, IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOff")]
    private static extern void SetNoteOffNative(IntPtr instancePtr, int instrumentId, float notePitch,
                                                double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOn")]
    private static extern void SetNoteOnNative(IntPtr instancePtr, int instrumentId, float notePitch,
                                               float noteIntensity, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParam")]
    private static extern void SetParamNative(IntPtr instancePtr, int instrumentId, int paramId, float paramValue,
                                              double timestamp);
  }
}
