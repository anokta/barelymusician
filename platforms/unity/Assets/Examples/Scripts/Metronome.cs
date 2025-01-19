using System;
using UnityEngine;

namespace Barely.Examples {
  /// Simple metronome that keeps track of beats.
  public class Metronome : MonoBehaviour {
    /// Beat count per bar.
    [Range(1, 16)]
    public int beatCount = 4;

    /// Bar note pitch.
    [Range(-1.0f, 1.0f)]
    public float barPitch = 1.0f;

    /// Beat note pitch.
    [Range(-1.0f, 1.0f)]
    public float beatPitch = 0.0f;

    /// Note intensity.
    [Range(0.0f, 1.0f)]
    public float intensity = 1.0f;

    /// True if logging to console, false otherwise.
    public bool isLoggingToConsole = false;

    /// True if ticking, false otherwise.
    public bool isTicking = false;

    /// True if playing on awake, false otherwise.
    public bool playOnAwake = false;

    /// True if auto, false otherwise.
    public bool isAuto = true;

    /// Position.
    public double Position {
      get { return _performer.Position + _beat; }
    }
    private int _beat = 0;

    /// Beat event callback.
    public delegate void BeatEventCallback(int bar, int beat);
    public event BeatEventCallback OnBeat;
    private BeatEventCallback _beatEventCallback = null;

    [Serializable]
    public class BeatEvent : UnityEngine.Events.UnityEvent<int, int> {}
    public BeatEvent OnBeatEvent;

    public bool IsPlaying {
      get { return _performer.IsPlaying; }
    }

    public void Play() {
      _performer.Play();
      if (isLoggingToConsole) {
        Debug.Log("Playback started");
      }
    }

    public void Pause() {
      _performer.Stop();
      if (isLoggingToConsole) {
        Debug.Log("Playback paused");
      }
    }

    public void Stop() {
      _performer.Stop();
      _performer.Position = 0.0;
      _beat = 0;
      if (isLoggingToConsole) {
        Debug.Log("Playback stopped");
      }
    }

    // Performer.
    private Performer _performer = null;

    private void Awake() {
      _beatEventCallback = delegate(int bar, int beat) {
        OnBeat?.Invoke(bar, beat);
        OnBeatEvent?.Invoke(bar, beat);
      };
    }

    private void OnDestroy() {
      _beatEventCallback = null;
    }

    private void OnEnable() {
      var tempGameObject = new GameObject() { hideFlags = HideFlags.HideAndDontSave };
      var instrument = tempGameObject.AddComponent<Instrument>();
      instrument.OscillatorShape = OscillatorShape.SQUARE;
      instrument.Gain = -12.0f;
      instrument.Attack = 0.0f;
      instrument.Release = 0.05f;
      _performer = tempGameObject.AddComponent<Performer>();
      _performer.PlayOnAwake = playOnAwake;
      _performer.Loop = true;
      _performer.LoopLength = 1.0;
      _performer.OnBeat += delegate() {
        int bar = _beat / beatCount;
        int beat = _beat % beatCount;
        float pitch = (beat == 0) ? barPitch : beatPitch;
        if (isTicking) {
          instrument.SetNoteOn(pitch, intensity);
          instrument.SetNoteOff(pitch);
        }
        if (isLoggingToConsole) {
          Debug.Log("Tick " + bar + "." + beat);
        }
        _beatEventCallback(bar, beat);
        ++_beat;
      };
    }

    private void OnDisable() {
      GameObject.Destroy(_performer.gameObject);
      _performer = null;
    }

    private void Update() {
      if (!isAuto) {
        return;
      }
      if (((Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer) &&
           Input.GetMouseButtonDown(0)) ||
          Input.GetKeyDown(KeyCode.Space)) {
        if (_performer.IsPlaying) {
          Pause();
        } else {
          Play();
        }
      } else if (Input.GetKeyDown(KeyCode.Backspace)) {
        Stop();
      }
    }
  }
}  // namespace Barely.Examples
