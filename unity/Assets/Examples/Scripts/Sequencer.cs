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
      public struct Note {
        [Range(0, 127)]
        public int key;
        [Range(0.0f, 1.0f)]
        public double intensity;
        [Min(0.0f)]
        public double position;
        [Min(0.0f)]
        public double duration;
      }
      public List<Note> notes = null;

      private Performer _performer = null;

      private void Awake() {
        _performer =
            new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<Performer>();
        _performer.playOnAwake = playOnAwake;
      }

      private void Update() {
        _performer.Tasks.Clear();
        for (int i = 0; i < notes.Count; ++i) {
          var note = notes[i];
          _performer.Tasks.Add(new Performer.Task(delegate() {
            var pitch = Musician.PitchFromMidiKey(note.key);
            instrument?.SetNoteOn(pitch, note.intensity);
            _performer.ScheduleTask(delegate() { instrument?.SetNoteOff(pitch); },
                                    _performer.Position + note.duration, -1);
          }, note.position));
        }
        _performer.Loop = loop;
        _performer.LoopLength = loopLength;
      }
    }
  }  // namespace Examples
}  // namespace Barely
