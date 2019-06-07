using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public class BarelyMusician {
    // Internal beat callback.
    public delegate void BeatCallback(int section, int bar, int beat);

    // Internal instrument callbacks.
    public delegate void ClearCallback();
    public delegate void NoteOffCallback(float index);
    public delegate void NoteOnCallback(float index, float intensity);
    public delegate void ProcessCallback([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)][In, Out] float[] output,
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

    // Registers sequencer beat callback.
    public void RegisterSequencerBeatCallback(BeatCallback beatCallback) {
      RegisterBeatCallback(Marshal.GetFunctionPointerForDelegate(beatCallback));
    }

    // Starts sequencer playback.
    public void PlaySequencer() {
      Start();
    }

    // Pauses sequencer playback.
    public void PauseSequencer() {
      Stop();
    }

    // Stops sequencer playback.
    public void StopSequencer() {
      Stop();
      SetPosition(0, 0, 0);
    }

    // Updates sequencer.
    public void UpdateSequencer() {
      Update();
    }

    // Sets sequencer transport.
    public void SetSequencerTransport(Sequencer sequencer) {
      SetNumBars(sequencer.numBars);
      SetNumBeats(sequencer.numBeats);
      SetTempo(sequencer.tempo);
    }

    // Creates new instrument.
    public int CreateInstrument(ClearCallback clearCallback, NoteOffCallback noteOffCallback,
                                NoteOnCallback noteOnCallback, ProcessCallback processCallback) {
      return CreateInstrument(Marshal.GetFunctionPointerForDelegate(clearCallback),
                              Marshal.GetFunctionPointerForDelegate(noteOffCallback),
                              Marshal.GetFunctionPointerForDelegate(noteOnCallback),
                              Marshal.GetFunctionPointerForDelegate(processCallback));
    }

    // Destroys instrument.
    public void DestroyInstrument(Instrument instrument) {
      DestroyInstrument(instrument.Id);
    }

    // Processes instrument.
    public void ProcessInstrument(Instrument instrument, float[] output) {
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

    // Sequencer handlers.
    [DllImport(pluginName)]
    private static extern void Start();

    [DllImport(pluginName)]
    private static extern void Stop();

    [DllImport(pluginName)]
    private static extern void Update();

    [DllImport(pluginName)]
    private static extern void RegisterBeatCallback(IntPtr beatCallbackPtr);

    [DllImport(pluginName)]
    private static extern void SetNumBars(int numBars);

    [DllImport(pluginName)]
    private static extern void SetNumBeats(int numBeats);

    [DllImport(pluginName)]
    private static extern void SetPosition(int section, int bar, int beat);

    [DllImport(pluginName)]
    private static extern void SetTempo(float tempo);

    // Instrument handlers.
    [DllImport(pluginName)]
    private static extern int CreateInstrument(IntPtr clearCallbackPtr, IntPtr noteOffCallbackPtr,
                                               IntPtr noteOnCallbackPtr, IntPtr processCallbackPtr);

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
  }
}
