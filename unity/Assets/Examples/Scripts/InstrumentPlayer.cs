using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that plays instrument.
public class InstrumentPlayer : MonoBehaviour {
  // Note.
  public float noteIndex = 0.0f;
  public float noteIntensity = 1.0f;

  public Performer performer;

  // Instrument to play.
  private IInstrument instrument = null;

  // Is playing?
  private bool isPlaying = false;

  void Start() {
    instrument = GetComponent<SineInstrument>();
    performer.Instrument = instrument;
  }

  //void Start() {
  //  for (int i = 0; i < 16; ++i) {
  //    float index = i % 4;
  //    instrument.ScheduleNoteOn(i, index, 1.0f);
  //    instrument.ScheduleNoteOff(i + 0.5, index);
  //  }
  //}

  void Update() {
    if (Input.GetKeyDown(KeyCode.S) && !isPlaying) {
      performer.NoteOn(noteIndex, noteIntensity);
      isPlaying = true;
    } else if (Input.GetKeyUp(KeyCode.S) && isPlaying) {
      performer.NoteOff(noteIndex);
      isPlaying = false;
    }
  }

}
