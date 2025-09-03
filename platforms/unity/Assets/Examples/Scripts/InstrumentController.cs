using UnityEngine;

namespace Barely.Examples {
  /// Instrument keyboard controller.
  public class InstrumentController : MonoBehaviour {
    // Instrument to control.
    public Instrument instrument = null;

    // Arpeggiator to control.
    public Arpeggiator arpeggiator = null;

    // Octave offset.
    [Range(-3, 3)]
    public int octaveOffset = 0;

    // Note gain.
    [Range(0.0f, 1.0f)]
    public float noteGain = 1.0f;

    // Ordered keys of one octave.
    private KeyCode[] _octaveKeys =
        new KeyCode[] { KeyCode.A, KeyCode.W, KeyCode.S, KeyCode.E, KeyCode.D,
                        KeyCode.F, KeyCode.T, KeyCode.G, KeyCode.Y, KeyCode.H,
                        KeyCode.U, KeyCode.J, KeyCode.K, KeyCode.O, KeyCode.L };

    void Update() {
      // Shift octaves.
      if (Input.GetKeyDown(KeyCode.Z)) {
        if (arpeggiator != null) {
          arpeggiator.SetAllNotesOff();
        }
        octaveOffset = Mathf.Max(octaveOffset - 1, -3);
      } else if (Input.GetKeyDown(KeyCode.X)) {
        if (arpeggiator != null) {
          arpeggiator.SetAllNotesOff();
        }
        octaveOffset = Mathf.Min(octaveOffset + 1, 3);
      }
      // Adjust note gain.
      if (Input.GetKeyDown(KeyCode.C)) {
        noteGain = Mathf.Max(noteGain - 0.2f, 0.0f);
      } else if (Input.GetKeyDown(KeyCode.V)) {
        noteGain = Mathf.Min(noteGain + 0.2f, 1.0f);
      }
      // Play notes.
      for (int i = 0; i < _octaveKeys.Length; ++i) {
        if (Input.GetKeyDown(_octaveKeys[i])) {
          if (arpeggiator != null) {
            arpeggiator.SetNoteOn(KeyIndexToPitch(i));
          } else if (instrument != null) {
            instrument.SetNoteOn(KeyIndexToPitch(i), noteGain);
          }
        } else if (Input.GetKeyUp(_octaveKeys[i])) {
          if (arpeggiator != null) {
            arpeggiator.SetNoteOff(KeyIndexToPitch(i));
          } else if (instrument != null) {
            instrument.SetNoteOff(KeyIndexToPitch(i));
          }
        }
      }
      if (Application.platform == RuntimePlatform.Android ||
          Application.platform == RuntimePlatform.IPhonePlayer) {
        if (Input.GetMouseButtonDown(0)) {
          instrument?.SetNoteOn(KeyIndexToPitch(0));
        } else if (Input.GetMouseButtonUp(0)) {
          instrument?.SetNoteOff(KeyIndexToPitch(0));
        }
      }
    }

    // Returns the corresponding pitch for the given key index.
    private float KeyIndexToPitch(int keyIndex) {
      return (float)octaveOffset + (float)keyIndex / 12.0f;
    }
  }
}  // namespace Barely.Examples
