using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {

  // Beat callback.
  public delegate void BeatCallback(int section, int bar, int beat);

  // Beat sequencer.
  [RequireComponent(typeof(AudioSource))]
  public class Sequencer : MonoBehaviour {
    // Tempo (BPM).
    [Range(0.0f, 480.0f)]
    public float tempo = 120.0f;

    // Number of bars per section.
    [Range(0, 16)]
    public int numBars = 0;

    // Number of beats per bar.
    [Range(0, 16)]
    public int numBeats = 0;

    void Awake() {
      BarelyMusician.Instance.RegisterSequencerBeatCallback(DebugBeatCallback);
    }

    void Update() {
      UpdateSequencer();
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.UpdateSequencer();
    }

    private void DebugBeatCallback(int section, int bar, int beat) {
      Debug.Log("Beat: " + section + "." + bar + "." + beat);
    }

    private void UpdateSequencer() {
      var barelymusician = BarelyMusician.Instance;
      barelymusician.SetSequencerTempo(tempo);
      barelymusician.SetSequencerNumBars(numBars);
      barelymusician.SetSequencerNumBeats(numBeats);
    }
  }

}  // namespace BarelyApi
