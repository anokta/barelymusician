using UnityEngine;

namespace Barely.Examples {
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

    private void OnNoteOff(float pitch) {
      Debug.Log(instrument.name + ": NoteOff(" + pitch.ToString("F1") + ")");
    }

    private void OnNoteOn(float pitch) {
      Debug.Log(instrument.name + ": NoteOn(" + pitch.ToString("F1") + ", " +
                instrument.GetNoteGain(pitch).ToString("F1") + ")");
    }
  }
}  // namespace Barely.Examples
