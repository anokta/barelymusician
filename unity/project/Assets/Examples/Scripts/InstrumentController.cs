using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Instrument keyboard controller.
public class InstrumentController : MonoBehaviour {
  // Instrument to control.
  public Instrument instrument = null;

  // Root note pitch.
  [Range(0.0f, 128.0f)]
  public float rootPitch = 60.0f;

  // Octave offset.
  [Range(-3, 3)]
  public int octaveOffset = 0;

  // Note intensity.
  [Range(0.0f, 1.0f)]
  public float noteIntensity = 1.0f;

  // Ordered keys of one octave.
  private KeyCode[] octaveKeys = new KeyCode[] { KeyCode.A, KeyCode.W, KeyCode.S, KeyCode.E, KeyCode.D, KeyCode.F,
                                                 KeyCode.T, KeyCode.G, KeyCode.Y, KeyCode.H, KeyCode.U, KeyCode.J,
                                                 KeyCode.K };

  void Update() {
    // Shift octaves.
    if (Input.GetKeyDown(KeyCode.Z)) {
      instrument.SetAllNotesOff();
      octaveOffset = Mathf.Max(octaveOffset - 1, -3);
    } else if (Input.GetKeyDown(KeyCode.X)) {
      instrument.SetAllNotesOff();
      octaveOffset = Mathf.Min(octaveOffset + 1, 3);
    }
    // Adjust note intensity.
    if (Input.GetKeyDown(KeyCode.C)) {
      noteIntensity = Mathf.Max(noteIntensity - 0.2f, 0.0f);
    } else if (Input.GetKeyDown(KeyCode.V)) {
      noteIntensity = Mathf.Min(noteIntensity + 0.2f, 1.0f);
    }
    // Play notes.
    for (int i = 0; i < octaveKeys.Length; ++i) {
      if (Input.GetKeyDown(octaveKeys[i])) {
        instrument.SetNoteOn(GetPitchFromKeyIndex(i), noteIntensity);
      } else if (Input.GetKeyUp(octaveKeys[i])) {
        instrument.SetNoteOff(GetPitchFromKeyIndex(i));
      }
    }
  }

  // Returns the corresponding pitch for the given key.
  private float GetPitchFromKeyIndex(int keyIndex) {
    return rootPitch + 12.0f * octaveOffset + keyIndex;
  }
}
