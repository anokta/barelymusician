using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Beat sequencer.
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

    // Is sequencer playing?
    public bool IsPlaying { get { return source != null && source.isPlaying; } }

    // Internal beat callback.
    private BarelyMusician.BeatCallback beatCallback = null;

    // Audio source.
    private AudioSource source = null;

    void Awake() {
      beatCallback = delegate (int section, int bar, int beat) {
        OnBeat?.Invoke(section, bar, beat);
      };
      source = gameObject.AddComponent<AudioSource>();
      source.hideFlags = HideFlags.HideAndDontSave | HideFlags.HideInInspector;
      source.bypassEffects = true;
      source.bypassListenerEffects = true;
      source.bypassReverbZones = true;
      source.playOnAwake = false;
      source.spatialBlend = 0.0f;
      source.Stop();
    }

    void OnEnable() {
      BarelyMusician.Instance.CreateSequencer(beatCallback);
    }

    void OnDisable() {
      BarelyMusician.Instance.DestroySequencer();
    }

    void OnDestroy() {
      beatCallback = null;
      Destroy(source);
      source = null;
    }

    void Update() {
      BarelyMusician.Instance.SetSequencerTransport(this);
      BarelyMusician.Instance.UpdateSequencer(this);
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.ProcessSequencer(this);
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
      BarelyMusician.Instance.ResetSequencer(this);
    }
  }
}
