using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Beat sequencer.
  [RequireComponent(typeof(AudioListener))]
  public class Sequencer : MonoBehaviour {
    // Beat event.
    public delegate void BeatEvent(int section, int bar, int beat);
    public event BeatEvent OnBeat;

    // Tempo (BPM).
    [Range(0.0f, 480.0f)]
    public float tempo = 120.0f;

    // Number of bars per section.
    [Range(0, 16)]
    public int numBars = 0;

    // Number of beats per bar.
    [Range(0, 16)]
    public int numBeats = 0;

    // Is sequencer playing?
    public bool IsPlaying { get; private set; }

    // Internal beat callback.
    private BarelyMusician.BeatCallback beatCallback = null;

    void Awake() {
      // TODO(#49): This should be revokable via OnEnable/OnDisable.
      beatCallback = delegate (int section, int bar, int beat) {
        OnBeat?.Invoke(section, bar, beat);
      };
      BarelyMusician.Instance.RegisterSequencerBeatCallback(beatCallback);
    }

    void Update() {
      BarelyMusician.Instance.SetSequencerTransport(this);
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.UpdateSequencer();
    }

    // Starts the sequencer.
    public void Play() {
      IsPlaying = true;
      BarelyMusician.Instance.PlaySequencer();
    }

    public void Pause() {
      IsPlaying = false;
      BarelyMusician.Instance.PauseSequencer();
    }

    // Stops the sequencer.
    public void Stop() {
      IsPlaying = false;
      BarelyMusician.Instance.StopSequencer();
    }
  }
}
