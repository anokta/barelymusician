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

      private void OnControl(int index, double value) {
        Debug.Log(instrument.name + ": Control(" + index + ", " + value.ToString("F1") + ")");
      }

      private void OnNoteControl(double pitch, int index, double value) {
        Debug.Log(instrument.name + ": NoteControl(" + pitch.ToString("F1") + ", " + index + ", " +
                  value.ToString("F1") + ")");
      }

      private void OnNoteOff(double pitch) {
        Debug.Log(instrument.name + ": NoteOff(" + pitch.ToString("F1") + ")");
      }

      private void OnNoteOn(double pitch, double intensity) {
        Debug.Log(instrument.name + ": NoteOn(" + pitch.ToString("F1") + ", " +
                  intensity.ToString("F1") + ")");
      }
    }
  }  // namespace Examples
}  // namespace Barely
