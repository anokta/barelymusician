using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// TODO: This is not really necessary now? Move all to BarelyMusician.

namespace BarelyApi {
  // Beat sequencer.
  public class Sequencer : MonoBehaviour {
    // Beat event.
    public delegate void BeatEvent(int beat);
    public event BeatEvent OnBeat;

    // Note off event.
    public delegate void NoteOffEvent(Performer instrument, float index);
    public event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(Performer instrument, float index, float intenstiy);
    public event NoteOnEvent OnNoteOn;

    // Tempo (BPM).
    [Range(0, 480)]
    public double tempo = 120.0;

    // Is sequencer playing?
    // TODO: Take this from native implementation.
    public bool IsPlaying { get { return source != null && source.isPlaying; } }

    // Internal beat callback.
    private BarelyMusician.BeatCallback beatCallback = null;

    // Internal note off callback.
    private BarelyMusician.NoteOffCallback noteOffCallback = null;

    // Internal note on callback.
    private BarelyMusician.NoteOnCallback noteOnCallback = null;

    // Audio source.
    private AudioSource source = null;

    void Awake() {
      beatCallback = delegate (int beat) {
        OnBeat?.Invoke(beat);
      };

      // TODO: Store instrument list rather than dynamic fetch!
      noteOffCallback = delegate (int id, float index) {
        var instruments = FindObjectsOfType<Performer>();
        Performer instrument = null;
        for (int i = 0; i < instruments.Length; ++i) {
          if (instruments[i]._id == id) {
            instrument = instruments[i];
            break;
          }
        }
        OnNoteOff?.Invoke(instrument, index);
      };
      noteOnCallback = delegate (int id, float index, float intensity) {
        var instruments = FindObjectsOfType<Performer>();
        Performer instrument = null;
        for (int i = 0; i < instruments.Length; ++i) {
          if (instruments[i]._id == id) {
            instrument = instruments[i];
            break;
          }
        }
        OnNoteOn?.Invoke(instrument, index, intensity);
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
      BarelyMusician.Instance.SetBeatCallback(beatCallback);
      BarelyMusician.Instance.SetNoteOffCallback(noteOffCallback);
      BarelyMusician.Instance.SetNoteOnCallback(noteOnCallback);
    }

    void OnDisable() {
      BarelyMusician.Instance.Stop();
      BarelyMusician.Instance.SetBeatCallback(null);
      BarelyMusician.Instance.SetNoteOffCallback(null);
      BarelyMusician.Instance.SetNoteOnCallback(null);
    }

    void OnDestroy() {
      beatCallback = null;
      noteOffCallback = null;
      noteOnCallback = null;
      Destroy(source);
      source = null;
    }

    void Update() {
      BarelyMusician.Instance.SetTempo(tempo);
      BarelyMusician.Instance.UpdateMainThread();
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.UpdateAudioThread();
    }

    // Returns playback position.
    public double GetPosition() {
      return BarelyMusician.Instance.GetPosition();
    }

    // Starts the sequencer.
    public void Play() {
      BarelyMusician.Instance.Start();
      source.Play();
    }

    // Pauses the sequencer.
    public void Pause() {
      source.Stop();
      BarelyMusician.Instance.Stop();
    }

    // Stops the sequencer.
    public void Stop() {
      source.Stop();
      BarelyMusician.Instance.Stop();
      BarelyMusician.Instance.SetPosition(0.0);
    }
  }
}
