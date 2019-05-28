using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  // Main |BarelyMusician| class that communicates with the native code.
  public class BarelyMusician {
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

    // Sets sequencer number of bars per section.
    public void SetSequencerNumBars(int numBars) {
      SetNumBars(numBars);
    }

    // Sets sequencer number of beats per bar.
    public void SetSequencerNumBeats(int numBeats) {
      SetNumBeats(numBeats);
    }

    // Sets sequencer tempo.
    public void SetSequencerTempo(float tempo) {
      SetTempo(tempo);
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

#if !UNITY_EDITOR && UNITY_IOS
  private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName)]
    private static extern void Initialize(int sampleRate, int numChannels, int numFrames);

    [DllImport(pluginName)]
    private static extern void RegisterBeatCallback(IntPtr beatCallback);

    [DllImport(pluginName)]
    private static extern void SetNumBars(int numBars);

    [DllImport(pluginName)]
    private static extern void SetNumBeats(int numBeats);

    [DllImport(pluginName)]
    private static extern void SetTempo(float tempo);

    [DllImport(pluginName)]
    private static extern void Shutdown();

    [DllImport(pluginName)]
    private static extern void Update();
  }
}  // namespace BarelyApi
