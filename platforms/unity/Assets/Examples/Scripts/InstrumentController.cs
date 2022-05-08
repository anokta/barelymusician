using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

// Instrument keyboard controller.
public class InstrumentController : MonoBehaviour {
  // Instrument to control.
  public Instrument instrument = null;

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
      instrument.StopAllNotes();
      octaveOffset = Mathf.Max(octaveOffset - 1, -3);
    } else if (Input.GetKeyDown(KeyCode.X)) {
      instrument.StopAllNotes();
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
        instrument.StartNote(GetPitchFromKeyIndex(i), noteIntensity);
      } else if (Input.GetKeyUp(octaveKeys[i])) {
        instrument.StopNote(GetPitchFromKeyIndex(i));
      }
    }
  }

  // Returns the corresponding pitch for the given key.
  private double GetPitchFromKeyIndex(int keyIndex) {
    return rootPitch + octaveOffset + (double)keyIndex / 12.0;
  }
}
