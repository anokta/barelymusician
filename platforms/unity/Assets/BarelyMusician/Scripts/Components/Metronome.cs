using System;
using UnityEngine;

/// Simple metronome that keeps track of beats.
namespace Barely {
  public class Metronome : MonoBehaviour {
    /// Global tempo in beats.
    [Range(30.0f, 960.0f)]
    public double tempo = 120.0;

    /// Instrument.
    public Instrument instrument = null;

    /// Beat count per bar.
    [Range(1, 16)]
    public int beatCount = 4;

    /// Bar note pitch.
    [Range(-4.0f, 4.0f)]
    public double barPitch = 1.0f;

    /// Beat note pitch.
    [Range(-4.0f, 4.0f)]
    public double beatPitch = 0.0f;

    /// Note intensity.
    [Range(0.0f, 1.0f)]
    public double intensity = 1.0f;

    /// True if logging to console, false otherwise.
    public bool isLoggingToConsole = true;

    /// True if ticking, false otherwise.
    public bool isTicking = false;

    /// Position.
    public double Position {
      get { return _performer.Position + _beat; }
    }
    private int _beat = 0;

    /// Beat event callback.
    public delegate void BeatEventCallback(int beat);
    public event BeatEventCallback OnBeat;
    private BeatEventCallback _beatEventCallback = null;

    [Serializable]
    public class BeatEvent : UnityEngine.Events.UnityEvent<int> {}
    public BeatEvent OnBeatEvent;

    private Performer _performer = null;

    private void Awake() {
      _beatEventCallback = delegate(int beat) {
        OnBeat?.Invoke(beat);
        OnBeatEvent?.Invoke(beat);
      };
      _performer =
          new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<Performer>();
      _performer.IsLooping = true;
      _performer.CreateTask(delegate() {
        int bar = _beat / beatCount;
        int beat = _beat % beatCount;
        double pitch = (beat == 0.0) ? barPitch : beatPitch;
        if (isTicking && instrument) {
          instrument.SetNoteOn(pitch, intensity);
          instrument.SetNoteOff(pitch);
        }
        if (isLoggingToConsole) {
          Debug.Log("Tick " + bar + "." + beat);
        }
        _beatEventCallback(beat);
        ++_beat;
      }, /*isOneOff=*/false, 0.0, -1);
    }

    private void Destroy() {
      _beatEventCallback = null;
      _performer = null;
    }

    private void Update() {
      Musician.Tempo = tempo;
      if (Input.GetKeyDown(KeyCode.Space)) {
        if (_performer.IsPlaying) {
          _performer.Stop();
          if (isLoggingToConsole) {
            Debug.Log("Playback paused");
          }
        } else {
          _performer.Play();
          if (isLoggingToConsole) {
            Debug.Log("Playback started");
          }
        }
      } else if (Input.GetKeyDown(KeyCode.Backspace)) {
        _performer.Stop();
        _performer.Position = 0.0;
        _beat = 0;
        if (isLoggingToConsole) {
          Debug.Log("Playback stopped");
        }
      }
    }
  }
}  // namespace Barely
