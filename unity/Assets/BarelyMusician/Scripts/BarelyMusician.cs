using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public class BarelyMusician {
    // Internal event callbacks.
    public delegate void BeatCallback(int section, int bar, int beat);

    // Internal instrument functions.
    public delegate void ClearFn();
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
    public int CreateInstrument(ClearFn clearFn, NoteOffFn noteOffFn,
                                NoteOnFn noteOnFn, ProcessFn processFn) {
      return CreateInstrument(Marshal.GetFunctionPointerForDelegate(clearFn),
                              Marshal.GetFunctionPointerForDelegate(noteOffFn),
                              Marshal.GetFunctionPointerForDelegate(noteOnFn),
                              Marshal.GetFunctionPointerForDelegate(processFn));
    }

    // Destroys instrument.
    public void DestroyInstrument(Instrument instrument) {
      DestroyInstrument(instrument.Id);
    }

    // Processes instrument.
    public void ProcessInstrument(Instrument instrument, float[] output) {
      Process();
      ProcessInstrument(instrument.Id, output);
    }

    // Clears instrument.
    public void SetInstrumentClear(Instrument instrument) {
      SetInstrumentClear(instrument.Id);
    }

    // Sets instrument note off.
    public void SetInstrumentNoteOff(Instrument instrument, float index) {
      SetInstrumentNoteOff(instrument.Id, index);
    }

    // Sets instrument note on.
    public void SetInstrumentNoteOn(Instrument instrument, float index, float intensity) {
      SetInstrumentNoteOn(instrument.Id, index, intensity);
    }

    // Updates instrument.
    public void UpdateInstrument() {
      Update();
    }

    // Creates sequencer.
    public int CreateSequencer(BeatCallback beatCallback) {
      return CreateSequencer(Marshal.GetFunctionPointerForDelegate(beatCallback));
    }

    // Destroys sequencer.
    public void DestroySequencer(Sequencer sequencer) {
      DestroySequencer(sequencer.Id);
    }

    // Processes sequencer.
    public void ProcessSequencer() {
      Process();
    }

    // Sets sequencer position.
    public void SetSequencerPosition(Sequencer sequencer, int section, int bar, int beat) {
      SetSequencerPosition(sequencer.Id, section, bar, beat);
    }

    // Sets sequencer transport.
    public void SetSequencerTransport(Sequencer sequencer) {
      SetSequencerNumBars(sequencer.Id, sequencer.numBars);
      SetSequencerNumBeats(sequencer.Id, sequencer.numBeats);
      SetSequencerTempo(sequencer.Id, sequencer.tempo);
    }

    // Starts sequencer playback.
    public void StartSequencer(Sequencer sequencer) {
      StartSequencer(sequencer.Id);
    }

    // Stops sequencer playback.
    public void StopSequencer(Sequencer sequencer) {
      StopSequencer(sequencer.Id);
    }

    // Updates sequencer.
    public void UpdateSequencer() {
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
    private void Process() {
      double dspTime = AudioSettings.dspTime;
      if (dspTime > lastDspTime) {
        lastDspTime = dspTime;
        UpdateAudioThread();
      }
    }

    // Updates the main thread state.
    private void Update() {
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

    [DllImport(pluginName)]
    private static extern void UpdateAudioThread();

    [DllImport(pluginName)]
    private static extern void UpdateMainThread();

    // Instrument handlers.
    [DllImport(pluginName)]
    private static extern int CreateInstrument(IntPtr clearFnPtr, IntPtr noteOffFnPtr,
                                               IntPtr noteOnFnPtr, IntPtr processFnPtr);

    [DllImport(pluginName)]
    private static extern void DestroyInstrument(int instrumentId);

    [DllImport(pluginName)]
    private static extern void ProcessInstrument(int instrumentId, [In, Out] float[] output);

    [DllImport(pluginName)]
    private static extern void SetInstrumentClear(int instrumentId);

    [DllImport(pluginName)]
    private static extern void SetInstrumentNoteOff(int instrumentId, float index);

    [DllImport(pluginName)]
    private static extern void SetInstrumentNoteOn(int instrumentId, float index, float intensity);

    // Sequencer handlers.
    [DllImport(pluginName)]
    private static extern int CreateSequencer(IntPtr beatCallbackPtr);

    [DllImport(pluginName)]
    private static extern void DestroySequencer(int sequencerId);

    [DllImport(pluginName)]
    private static extern void SetSequencerNumBars(int sequencerId, int numBars);

    [DllImport(pluginName)]
    private static extern void SetSequencerNumBeats(int sequencerId, int numBeats);

    [DllImport(pluginName)]
    private static extern void SetSequencerPosition(int sequencerId, int section, int bar, int beat);

    [DllImport(pluginName)]
    private static extern void SetSequencerTempo(int sequencerId, float tempo);

    [DllImport(pluginName)]
    private static extern void StartSequencer(int sequencerId);

    [DllImport(pluginName)]
    private static extern void StopSequencer(int sequencerId);
  }
}
