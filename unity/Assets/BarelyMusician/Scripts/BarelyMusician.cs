using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public class BarelyMusician {
    // Internal event callbacks.
    public delegate void BeatCallback(int beat);
    public delegate void NoteOffCallback(int id, float index);
    public delegate void NoteOnCallback(int id, float index, float intensity);

    // Internal Unity instrument functions.
    public delegate void NoteOffFn(float index);
    public delegate void NoteOnFn(float index, float intensity);
    public delegate void ProcessFn(
      [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output, int size,
      int numChannels);

    // Invalid ID.
    public const int InvalidId = -1;

    // Singleton instance.
    public static BarelyMusician Instance {
      get {
        if (instance == null) {
          instance = new BarelyMusician();
        }
        return instance;
      }
    }
    private static BarelyMusician instance = null;

    // Audio thread last DSP time.
    private double lastDspTime = 0.0;

    // Main thread last update time.
    private float lastUpdateTime = 0.0f;

    // Creates new instrument.
    public int Create() {
      return CreateNative();
    }

    public void SetInstrument(int id, IInstrument instrument) {
      if (instrument == null) {
        // TODO: Set null.
      }
      if (instrument.GetType().IsSubclassOf(typeof(UnityInstrument))) {
        var unityInstrument = instrument as UnityInstrument;
        SetUnityInstrumentNative(id, 
                                 Marshal.GetFunctionPointerForDelegate(unityInstrument.noteOffFn),
                                 Marshal.GetFunctionPointerForDelegate(unityInstrument.noteOnFn),
                                 Marshal.GetFunctionPointerForDelegate(unityInstrument.processFn));
      } else {
        Debug.LogWarning("Instrument type not supported: " + instrument.GetType());
      }
    }


    // Destroys instrument.
    public void Destroy(int id) {
      DestroyNative(id);
    }

    // Returns playback position.
    public double GetPosition() {
      return GetPositionNative();
    }

    // Processes instrument.
    public void Process(int id, float[] output) {
      ProcessNative(id, output);
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

    // Sets beat callback.
    public void SetBeatCallback(BeatCallback beatCallback) {
      IntPtr beatCallbackPtr = (beatCallback != null)
                                  ? Marshal.GetFunctionPointerForDelegate(beatCallback)
                                  : IntPtr.Zero;
      SetBeatCallbackNative(beatCallbackPtr);
    }

    // Sets note off callback.
    public void SetNoteOffCallback(NoteOffCallback noteOffCallback) {
      IntPtr noteOffCallbackPtr = (noteOffCallback != null)
                            ? Marshal.GetFunctionPointerForDelegate(noteOffCallback)
                            : IntPtr.Zero;
      SetNoteOffCallbackNative(noteOffCallbackPtr);
    }

    // Sets note on callback.
    public void SetNoteOnCallback(NoteOnCallback noteOnCallback) {
      IntPtr noteOnCallbackPtr = (noteOnCallback != null)
                            ? Marshal.GetFunctionPointerForDelegate(noteOnCallback)
                            : IntPtr.Zero;
      SetNoteOnCallbackNative(noteOnCallbackPtr);
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
    public void Start() {
      StartNative();
    }

    // Stops playback.
    public void Stop() {
      StopNative();
    }

    // Constructs new |BarelyMusician| with Unity audio settings.                                                       
    BarelyMusician() {
      var config = AudioSettings.GetConfiguration();
      int sampleRate = config.sampleRate;
      int numChannels = (int)config.speakerMode;
      int numFrames = config.dspBufferSize;
      InitializeNative(sampleRate, numChannels, numFrames);
    }

    // Shuts down |BarelyMusician|.
    ~BarelyMusician() {
      ShutdownNative();
    }

    // Updates the audio thread state.
    public void UpdateAudioThread() {
      double dspTime = AudioSettings.dspTime;
      if (dspTime > lastDspTime) {
        lastDspTime = dspTime;
        UpdateAudioThreadNative();
      }
    }

    // Updates the main thread state.
    public void UpdateMainThread() {
      float updateTime = Time.unscaledTime;
      if (updateTime > lastUpdateTime) {
        lastUpdateTime = updateTime;
        UpdateMainThreadNative();
      }
    }

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "Initialize")]
    private static extern void InitializeNative(int sampleRate, int numChannels, int numFrames);

    [DllImport(pluginName, EntryPoint = "Shutdown")]
    private static extern void ShutdownNative();

    [DllImport(pluginName, EntryPoint = "Create")]
    private static extern int CreateNative();

    [DllImport(pluginName, EntryPoint = "Destroy")]
    private static extern void DestroyNative(int id);

    [DllImport(pluginName, EntryPoint = "GetPosition")]
    private static extern double GetPositionNative();

    [DllImport(pluginName, EntryPoint = "Process")]
    private static extern void ProcessNative(int id, [In, Out] float[] output);

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

    [DllImport(pluginName, EntryPoint = "SetUnityInstrument")]
    private static extern int SetUnityInstrumentNative(int id, IntPtr noteOffFnPtr,
                                                       IntPtr noteOnFnPtr, IntPtr processFnPtr);

    [DllImport(pluginName, EntryPoint = "Start")]
    private static extern void StartNative();

    [DllImport(pluginName, EntryPoint = "Stop")]
    private static extern void StopNative();

    [DllImport(pluginName, EntryPoint = "UpdateAudioThread")]
    private static extern void UpdateAudioThreadNative();

    [DllImport(pluginName, EntryPoint = "UpdateMainThread")]
    private static extern void UpdateMainThreadNative();
  }
}
