using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that plays instrument.
public class InstrumentPlayer : MonoBehaviour {
  // Note.
  public float noteIndex = 0.0f;
  public float noteIntensity = 1.0f;

  // Instrument to play.
  public Instrument instrument;

  // Is playing?
  private bool isPlaying = false;

  void Start() {
    for (int i = 0; i < 16; ++i) {
      float index = i % 4;
      instrument.ScheduleNoteOn(i, index, 1.0f);
      instrument.ScheduleNoteOff(i + 0.5, index);
    }
  }

  void Update() {
    if (Input.GetKeyDown(KeyCode.S) && !isPlaying) {
      instrument.NoteOn(noteIndex, noteIntensity);
      isPlaying = true;
    } else if (Input.GetKeyUp(KeyCode.S) && isPlaying) {
      instrument.NoteOff(noteIndex);
      isPlaying = false;
    }
  }

}
