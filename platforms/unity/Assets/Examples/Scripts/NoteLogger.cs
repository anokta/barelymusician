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

      private void OnNoteControl(int pitch, int id, double value) {
        Debug.Log(instrument.name + ": NoteControl(" + pitch + ", " + id + ", " +
                  value.ToString("F2") + ")");
      }

      private void OnNoteOff(int pitch) {
        Debug.Log(instrument.name + ": NoteOff(" + pitch + ")");
      }

      private void OnNoteOn(int pitch, double intensity) {
        Debug.Log(instrument.name + ": NoteOn(" + pitch + ", " + intensity.ToString("F1") + ")");
      }
    }
  }  // namespace Examples
}  // namespace Barely
