using UnityEngine;

namespace Barely {
  namespace Examples {
    /// Instrument keyboard controller.
    public class InstrumentController : MonoBehaviour {
      // Instrument to control.
      public Instrument instrument = null;

      // Arpeggiator to control.
      public Arpeggiator arpeggiator = null;

      // Repeater to control.
      public Repeater repeater = null;

      // Root note pitch.
      [Range(0.0f, 1.0f)]
      public double rootPitch = 0.0;

      // Octave offset.
      [Range(-3, 3)]
      public int octaveOffset = 0;

      // Note intensity.
      [Range(0.0f, 1.0f)]
      public float noteIntensity = 1.0f;

      [Range(1, 4)]
      public int repeaterNoteLength = 1;

      // Ordered keys of one octave.
      private KeyCode[] _octaveKeys =
          new KeyCode[] { KeyCode.A, KeyCode.W, KeyCode.S, KeyCode.E, KeyCode.D,
                          KeyCode.F, KeyCode.T, KeyCode.G, KeyCode.Y, KeyCode.H,
                          KeyCode.U, KeyCode.J, KeyCode.K, KeyCode.O, KeyCode.L };

      // True if the repeater is active, false otherwise.
      private bool _isRepeaterActive = false;

      void Update() {
        if (repeater != null) {
          if (Input.GetKeyDown(KeyCode.Space)) {
            _isRepeaterActive = !_isRepeaterActive;
            if (_isRepeaterActive) {
              repeater.Play();
            } else {
              repeater.Stop();
            }
          } else if (Input.GetKeyDown(KeyCode.Backspace)) {
            repeater.Pop();
          } else if (!_isRepeaterActive) {
            if (Input.GetKeyDown(KeyCode.Alpha0)) {
              repeater.Push(null, repeaterNoteLength);
            }
          }
        }
        // Shift octaves.
        if (Input.GetKeyDown(KeyCode.Z)) {
          if (arpeggiator != null) {
            arpeggiator.SetAllNotesOff();
          } else if (!_isRepeaterActive && instrument != null) {
            instrument.SetAllNotesOff();
          }
          octaveOffset = Mathf.Max(octaveOffset - 1, -3);
        } else if (Input.GetKeyDown(KeyCode.X)) {
          if (arpeggiator != null) {
            arpeggiator.SetAllNotesOff();
          } else if (!_isRepeaterActive && instrument != null) {
            instrument.SetAllNotesOff();
          }
          octaveOffset = Mathf.Min(octaveOffset + 1, 3);
        }
        // Adjust note intensity.
        if (Input.GetKeyDown(KeyCode.C)) {
          noteIntensity = Mathf.Max(noteIntensity - 0.2f, 0.0f);
        } else if (Input.GetKeyDown(KeyCode.V)) {
          noteIntensity = Mathf.Min(noteIntensity + 0.2f, 1.0f);
        }
        // Play notes.
        for (int i = 0; i < _octaveKeys.Length; ++i) {
          if (Input.GetKeyDown(_octaveKeys[i])) {
            if (arpeggiator != null) {
              arpeggiator.SetNoteOn(GetPitchFromKeyIndex(i));
            } else if (_isRepeaterActive) {
              repeater.Stop();
              repeater.Play(GetPitchFromKeyIndex(i) - rootPitch);
            } else if (instrument != null) {
              instrument.SetNoteOn(GetPitchFromKeyIndex(i), noteIntensity);
              if (repeater != null) {
                repeater.Push(GetPitchFromKeyIndex(i), repeaterNoteLength);
              }
            }
          } else if (Input.GetKeyUp(_octaveKeys[i])) {
            if (arpeggiator != null) {
              arpeggiator.SetNoteOff(GetPitchFromKeyIndex(i));
            } else if (instrument != null) {
              instrument.SetNoteOff(GetPitchFromKeyIndex(i));
            }
          }
        }
        if (Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer) {
          if (Input.GetMouseButtonDown(0)) {
            instrument?.SetNoteOn(0.0);
          } else if (Input.GetMouseButtonUp(0)) {
            instrument?.SetNoteOff(0.0);
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
