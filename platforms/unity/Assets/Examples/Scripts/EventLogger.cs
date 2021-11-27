using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class EventLogger : MonoBehaviour {
  private void OnEnable() {
    Musician.OnInstrumentNoteOff += OnNoteOff;
    Musician.OnInstrumentNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    Musician.OnInstrumentNoteOff -= OnNoteOff;
    Musician.OnInstrumentNoteOn -= OnNoteOn;
  }

  private void OnNoteOff(Instrument instrument, float note_pitch) {
    Debug.Log(instrument.name + ": NoteOff(" + note_pitch + ")");
  }

  private void OnNoteOn(Instrument instrument, float note_pitch, float note_intensity) {
    Debug.Log(instrument.name + ": NoteOn(" + note_pitch + ", " + note_intensity + ")");
  }
}
