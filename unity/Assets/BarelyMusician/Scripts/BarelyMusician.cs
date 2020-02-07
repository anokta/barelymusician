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
    public delegate void ProcessFn([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output,
                                   int size, int numChannels);

    // Invalid ID.
    public static readonly int InvalidId = -1;

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
    public int Create(NoteOffFn noteOffFn, NoteOnFn noteOnFn, ProcessFn processFn) {
      return Create(Marshal.GetFunctionPointerForDelegate(noteOffFn), Marshal.GetFunctionPointerForDelegate(noteOnFn),
                    Marshal.GetFunctionPointerForDelegate(processFn));
    }

    // Destroys instrument.
    public void Destroy(Instrument instrument) {
      Destroy(instrument.Id);
    }

    // Processes instrument.
    public void Process(Instrument instrument, float[] output) {
      Process();
      Process(instrument.Id, output);
    }

    // Sets instrument note off.
    public void NoteOff(Instrument instrument, float index) {
      NoteOff(instrument.Id, index);
    }

    // Sets instrument note on.
    public void NoteOn(Instrument instrument, float index, float intensity) {
      NoteOn(instrument.Id, index, intensity);
    }

    // Schedules instrument note off.
    public void ScheduleNoteOff(Instrument instrument, float index, double position) {
      ScheduleNoteOff(instrument.Id, index, position);
    }

    // Schedules instrument note on.
    public void ScheduleNoteOn(Instrument instrument, float index, float intensity, double position) {
      ScheduleNoteOn(instrument.Id, index, intensity, position);
    }

    // Sets beat callback.
    public void SetBeatCallback(BeatCallback beatCallback)
    {
      IntPtr beatCallbackPtr = 
          (beatCallback != null) ? Marshal.GetFunctionPointerForDelegate(beatCallback) : IntPtr.Zero; 
      SetBeatCallback(beatCallbackPtr);
    }

    // Sets note off callback.
    public void SetNoteOffCallback(NoteOffCallback noteOffCallback) {
      IntPtr noteOffCallbackPtr =
          (noteOffCallback != null) ? Marshal.GetFunctionPointerForDelegate(noteOffCallback) : IntPtr.Zero;
      SetNoteOffCallback(noteOffCallbackPtr);
    }
    
    // Sets note on callback.
    public void SetNoteOnCallback(NoteOnCallback noteOnCallback) {
      IntPtr noteOnCallbackPtr =
          (noteOnCallback != null) ? Marshal.GetFunctionPointerForDelegate(noteOnCallback) : IntPtr.Zero;
      SetNoteOnCallback(noteOnCallbackPtr);
    }

    // Starts playback.
    public void Start(Sequencer sequencer) {
      Start();
    }

    // Stops playback.
    public void Stop(Sequencer sequencer) {
      Stop();
    }

    // Updates sequencer.
    public void Update(Sequencer sequencer) {
      SetTempo(sequencer.tempo);
      Update();
    }

    // Constructs new |BarelyMusician| with Unity audio settings.
    BarelyMusician() {
      var config = AudioSettings.GetConfiguration();
      int sampleRate = config.sampleRate;
      int numChannels = (int)config.speakerMode;
      int numFrames = config.dspBufferSize;
      Initialize(sampleRate, numChannels, numFrames);
    }

    // Shuts down |BarelyMusician|.
    ~BarelyMusician() {
      Shutdown();
    }

    // Updates the audio thread state.
    public void Process() {
      double dspTime = AudioSettings.dspTime;
      if (dspTime > lastDspTime) {
        lastDspTime = dspTime;
        UpdateAudioThread();
      }
    }

    // Updates the main thread state.
    public void Update() {
      float updateTime = Time.unscaledTime;
      if (updateTime > lastUpdateTime) {
        lastUpdateTime = updateTime;
        UpdateMainThread();
      }
    }

#if !UNITY_EDITOR && UNITY_IOS
  private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    // System handlers.
    [DllImport(pluginName)]
    private static extern void Initialize(int sampleRate, int numChannels, int numFrames);

    [DllImport(pluginName)]
    private static extern void Shutdown();

    // Instrument handlers.
    [DllImport(pluginName)]
    private static extern int Create(IntPtr noteOffFnPtr, IntPtr noteOnFnPtr, IntPtr processFnPtr);

    [DllImport(pluginName)]
    private static extern void Destroy(int id);

    [DllImport(pluginName)]
    private static extern void Process(int id, [In, Out] float[] output);

    [DllImport(pluginName)]
    private static extern void NoteOff(int id, float index);

    [DllImport(pluginName)]
    private static extern void NoteOn(int id, float index, float intensity);

    [DllImport(pluginName)]
    private static extern void ScheduleNoteOff(int id, float index, double position);

    [DllImport(pluginName)]                                        
    private static extern void ScheduleNoteOn(int id, float index, float intensity, double position);

    [DllImport(pluginName)]
    private static extern void SetBeatCallback(IntPtr beatCallbackPtr);

    [DllImport(pluginName)]
    private static extern void SetNoteOffCallback(IntPtr noteOffCallbackPtr);

    [DllImport(pluginName)]
    private static extern void SetNoteOnCallback(IntPtr noteOnCallbackPtr);

    [DllImport(pluginName)]
    private static extern void SetTempo(double tempo);

    [DllImport(pluginName)]
    private static extern void Start();

    [DllImport(pluginName)]
    private static extern void Stop();

    [DllImport(pluginName)]
    private static extern void UpdateAudioThread();

    [DllImport(pluginName)]
    private static extern void UpdateMainThread();
  }
}
