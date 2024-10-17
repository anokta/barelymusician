using UnityEngine;

namespace Barely {
  namespace Examples {
    public class NoteLogger : MonoBehaviour {
      private Instrument instrument = null;

      private void OnEnable() {
        instrument = GetComponent<Instrument>();
        instrument.OnNoteOff += OnNoteOff;
        instrument.OnNoteOn += OnNoteOn;
      }

      private void OnDisable() {
        instrument = GetComponent<Instrument>();
        instrument.OnNoteOff -= OnNoteOff;
        instrument.OnNoteOn -= OnNoteOn;
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
