using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  namespace Examples {
    public class Sequencer : MonoBehaviour {
      public Instrument instrument = null;

      public bool loop = true;

      [Min(0.0f)]
      public double loopLength = 1.0;

      public bool playOnAwake = true;

      [System.Serializable]
      public class Note {
        [Range(0, 127)]
        public int key;
        [Range(0.0f, 1.0f)]
        public float intensity;
        [Min(0.0f)]
        public double position;
        [Min(0.0f)]
        public double duration;
        public bool muted;
      }
      public List<Note> notes = null;
      private List<Note> _notes = null;

      public bool IsPlaying {
        get { return _performer.IsPlaying; }
      }

      public void Play() {
        _performer.Play();
      }

      public void Pause() {
        _performer.Stop();
        instrument.SetAllNotesOff();
      }

      public void Stop() {
        _performer.Stop();
        _performer.Position = 0.0;
        instrument.SetAllNotesOff();
      }

      private Performer _performer = null;

      private void Awake() {
        _performer =
            new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<Performer>();
        _performer.PlayOnAwake = playOnAwake;
      }

      private void OnEnable() {
        UpdateNotes();
      }

      private void OnDestroy() {
        GameObject.Destroy(_performer.gameObject);
        _performer = null;
      }

      private void Update() {
        if (_notes != notes) {
          UpdateNotes();
        }
        _performer.Loop = loop;
        _performer.LoopLength = loopLength;
      }

      private void UpdateNotes() {
        _notes = notes;
        _performer.Tasks.Clear();
        for (int i = 0; i < _notes.Count; ++i) {
          var note = _notes[i];
          if (note.muted) {
            continue;
          }
          _performer.Tasks.Add(new Task(delegate() {
            var pitch = Musician.PitchFromMidiKey(note.key);
            instrument?.SetNoteOn(pitch, note.intensity);
            _performer.ScheduleOneOffTask(delegate() { instrument?.SetNoteOff(pitch); },
                                          _performer.Position + note.duration, -1);
          }, note.position));
        }
      }
    }
  }  // namespace Examples
}  // namespace Barely
