using UnityEngine;

namespace Barely {
  namespace Examples {
    public class NoteLogger : MonoBehaviour {
      private Instrument instrument = null;

      private void OnEnable() {
        instrument = GetComponent<Instrument>();
        instrument.OnControl += OnControl;
        instrument.OnNoteControl += OnNoteControl;
        instrument.OnNoteOff += OnNoteOff;
        instrument.OnNoteOn += OnNoteOn;
      }

      private void OnDisable() {
        instrument = GetComponent<Instrument>();
        instrument.OnControl -= OnControl;
        instrument.OnNoteControl -= OnNoteControl;
        instrument.OnNoteOff -= OnNoteOff;
        instrument.OnNoteOn -= OnNoteOn;
      }

      private void OnControl(int id, double value) {
        Debug.Log(instrument.name + ": Control(" + id + ", " + value.ToString("F2") + ")");
      }

      private void OnNoteControl(double note, int id, double value) {
        Debug.Log(instrument.name + ": NoteControl(" + note.ToString("F2") + id + ", " +
                  value.ToString("F2") + ")");
      }

      private void OnNoteOff(double note) {
        Debug.Log(instrument.name + ": NoteOff(" + note.ToString("F2") + ")");
      }

      private void OnNoteOn(double note, double intensity) {
        Debug.Log(instrument.name + ": NoteOn(" + note.ToString("F2") + ", " +
                  intensity.ToString("F1") + ")");
      }
    }
  }  // namespace Examples
}  // namespace Barely
