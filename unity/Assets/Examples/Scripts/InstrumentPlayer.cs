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

  void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
  }

  void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
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

  void OnBeat(int beat) {
    float index = beat % 4;
    // TODO: Too late to get in the beat without offset?
    instrument.ScheduleNoteOn(beat + 1, index, 1.0f);
    instrument.ScheduleNoteOff(beat + 1.5, index);

    if ((beat + 1) % 8 == 7) {
      instrument.ScheduleNoteOn(beat + 1.5, 0, 0.75f);
      instrument.ScheduleNoteOff(beat + 1.75, 0);
      instrument.ScheduleNoteOn(beat + 1.75, 2, 0.725f);
      instrument.ScheduleNoteOff(beat + 2.0, 2);
    }
  }
}
