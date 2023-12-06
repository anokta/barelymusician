using UnityEngine;

namespace Barely {
  namespace Examples {
    // Instrument keyboard controller.
    public class InstrumentController : MonoBehaviour {
      // Instrument to control.
      public Instrument instrument = null;

      public Arpeggiator arpeggiator = null;

      // Root note pitch.
      [Range(0.0f, 1.0f)]
      public double rootPitch = 0.0;

      // Octave offset.
      [Range(-3, 3)]
      public int octaveOffset = 0;

      // Note intensity.
      [Range(0.0f, 1.0f)]
      public double noteIntensity = 1.0;

      // Ordered keys of one octave.
      private KeyCode[] octaveKeys =
          new KeyCode[] { KeyCode.A, KeyCode.W, KeyCode.S, KeyCode.E, KeyCode.D,
                          KeyCode.F, KeyCode.T, KeyCode.G, KeyCode.Y, KeyCode.H,
                          KeyCode.U, KeyCode.J, KeyCode.K, KeyCode.O, KeyCode.L };

      void Update() {
        // Shift octaves.
        if (Input.GetKeyDown(KeyCode.Z)) {
          if (arpeggiator != null) {
            arpeggiator.SetAllNotesOff();
          } else {
            instrument.SetAllNotesOff();
          }
          octaveOffset = Mathf.Max(octaveOffset - 1, -3);
        } else if (Input.GetKeyDown(KeyCode.X)) {
          if (arpeggiator != null) {
            arpeggiator.SetAllNotesOff();
          } else {
            instrument.SetAllNotesOff();
          }
          octaveOffset = Mathf.Min(octaveOffset + 1, 3);
        }
        // Adjust note intensity.
        if (Input.GetKeyDown(KeyCode.C)) {
          noteIntensity = (double)Mathf.Max((float)noteIntensity - 0.2f, 0.0f);
        } else if (Input.GetKeyDown(KeyCode.V)) {
          noteIntensity = (double)Mathf.Min((float)noteIntensity + 0.2f, 1.0f);
        }
        // Play notes.
        for (int i = 0; i < octaveKeys.Length; ++i) {
          if (Input.GetKeyDown(octaveKeys[i])) {
            if (arpeggiator != null) {
              arpeggiator.SetNoteOn(GetPitchFromKeyIndex(i));
            } else {
              instrument.SetNoteOn(GetPitchFromKeyIndex(i), noteIntensity);
            }
          } else if (Input.GetKeyUp(octaveKeys[i])) {
            if (arpeggiator != null) {
              arpeggiator.SetNoteOff(GetPitchFromKeyIndex(i));
            } else {
              instrument.SetNoteOff(GetPitchFromKeyIndex(i));
            }
          }
        }
        if (Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer) {
          if (Input.GetMouseButtonDown(0)) {
            instrument.SetNoteOn(0.0);
          } else if (Input.GetMouseButtonUp(0)) {
            instrument.SetNoteOff(0.0);
          }
        }
      }

      // Returns the corresponding pitch for the given key.
      private double GetPitchFromKeyIndex(int keyIndex) {
        return rootPitch + octaveOffset + (double)keyIndex / 12.0;
      }
    }
  }  // namespace Examples
}  // namespace Barely
