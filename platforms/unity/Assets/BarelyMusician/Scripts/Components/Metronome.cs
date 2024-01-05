using System;
using UnityEngine;

namespace Barely {
  namespace Examples {
    /// Simple metronome that keeps track of beats.
    public class Metronome : MonoBehaviour {
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

      // Process order.
      private const int ProcessOrder = -1000;

      // Performer.
      private Performer _performer = null;

      private void Awake() {
        _beatEventCallback = delegate(int bar, int beat) {
          OnBeat?.Invoke(bar, beat);
          OnBeatEvent?.Invoke(bar, beat);
        };
        var tempGameObject = new GameObject() { hideFlags = HideFlags.HideAndDontSave };
        var instrument = tempGameObject.AddComponent<SynthInstrument>();
        instrument.OscillatorType = OscillatorType.SQUARE;
        instrument.Gain = 0.25;
        instrument.Attack = 0.0;
        instrument.Release = 0.05;
        _performer = tempGameObject.AddComponent<Performer>();
        _performer.playOnAwake = playOnAwake;
        _performer.Loop = true;
        _performer.LoopLength = 1.0;
        _performer.Tasks.Add(new Performer.Task(delegate() {
          int bar = _beat / beatCount;
          int beat = _beat % beatCount;
          double pitch = (beat == 0.0) ? barPitch : beatPitch;
          if (isTicking) {
            instrument.SetNoteOn(pitch, intensity);
            instrument.SetNoteOff(pitch);
          }
          if (isLoggingToConsole) {
            Debug.Log("Tick " + bar + "." + beat);
          }
          _beatEventCallback(bar, beat);
          ++_beat;
        }, 0.0, ProcessOrder));
      }

      private void OnDestroy() {
        GameObject.Destroy(_performer.gameObject);
        _performer = null;
        _beatEventCallback = null;
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
  }  // namespace Examples
}  // namespace Barely
