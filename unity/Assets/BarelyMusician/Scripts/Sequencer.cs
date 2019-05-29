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
    public int numBars = 0;

    // Number of beats per bar.
    [Range(0, 16)]
    public int numBeats = 0;

    // Sequencer ID.
    public int Id { get; private set; }

    public bool IsPlaying {
      get { return audioSource != null && audioSource.isPlaying; }
    }

    // Audio source.
    private AudioSource audioSource = null;

    // Internal beat callback.
    private BarelyMusician.BeatCallback beatCallback = null;

    void Awake() {
      audioSource = GetComponent<AudioSource>();
    }

    void OnEnable() {
      beatCallback = delegate (int section, int bar, int beat) {
        if (OnBeat != null) {
          OnBeat(section, bar, beat);
        }
      };
      Id = BarelyMusician.Instance.CreateSequencer(beatCallback);
    }

    void OnDisable() {
      BarelyMusician.Instance.DestroySequencer(this);
      Id = BarelyMusician.InvalidId;
    }

    void Update() {
      BarelyMusician.Instance.SetSequencerTransport(this);
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.ProcessSequencer(this);
    }

    // Starts the sequencer.
    public void Play() {
      audioSource.Play();
    }

    // Stops the sequencer.
    public void Stop() {
      audioSource.Stop();
    }
  }
}
