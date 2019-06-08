using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Beat sequencer.
  [RequireComponent(typeof(AudioSource))]
  public class Sequencer : MonoBehaviour {
    // Beat event.
    public delegate void BeatEvent(int section, int bar, int beat);
    public event BeatEvent OnBeat;

    // Tempo (BPM).
    [Range(0.0f, 480.0f)]
    public float tempo = 120.0f;

    // Number of bars per section.
    [Range(0, 16)]
    public int numBars = 4;

    // Number of beats per bar.
    [Range(0, 16)]
    public int numBeats = 4;

    // Instrument ID.
    public int Id {
      get;
      private set;
    }

    // Is sequencer playing?
    public bool IsPlaying { get; private set; }

    // Internal beat callback.
    private BarelyMusician.BeatCallback beatCallback = null;

    // Audio source.
    private AudioSource source = null;

    void Awake() {
      beatCallback = delegate (int section, int bar, int beat) {
        OnBeat?.Invoke(section, bar, beat);
      };
      source = GetComponent<AudioSource>();
      source.Stop();
    }

    void OnEnable() {
      Id = BarelyMusician.Instance.CreateSequencer(beatCallback);
    }

    private void OnDisable() {
      BarelyMusician.Instance.DestroySequencer(this);
      Id = BarelyMusician.InvalidId;
    }

    void Update() {
      BarelyMusician.Instance.SetSequencerTransport(this);
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.UpdateSequencer();
    }

    // Starts the sequencer.
    public void Play() {
      BarelyMusician.Instance.StartSequencer(this);
      source.Play();
    }

    // Pauses the sequencer.
    public void Pause() {
      source.Stop();
      BarelyMusician.Instance.StopSequencer(this);
    }

    // Stops the sequencer.
    public void Stop() {
      source.Stop();
      BarelyMusician.Instance.StopSequencer(this);
      BarelyMusician.Instance.SetSequencerPosition(this, 0, 0, 0);
    }
  }
}
