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
  private Instrument instrument = null;

  // Is playing?
  private bool isPlaying = false;

  void Awake() {
    instrument = GetComponent<SineInstrument>();
  }

  //void Start() {
  //  for (int i = 0; i < 20; ++i) {
  //    instrument.ScheduleNoteOn((i % 4), 1.0f, i);
  //    instrument.ScheduleNoteOff((i % 4), i + 0.5);
  //  }
  //  //instrument.ScheduleNoteOn(1.0f, 1.0f, 1.0);
  //  //instrument.ScheduleNoteOff(1.0f, 1.5);
  //  //instrument.ScheduleNoteOn(2.0f, 1.0f, 2.0);
  //  //instrument.ScheduleNoteOff(2.0f, 2.5);
  //}

  void Update() {
    if (Input.GetKeyDown(KeyCode.S) && !isPlaying) {
      instrument.NoteOn(noteIndex, noteIntensity);
      isPlaying = true;
    }
    else if (Input.GetKeyUp(KeyCode.S) && isPlaying) {
      instrument.NoteOff(noteIndex);
      isPlaying = false;
    }
  }

}
