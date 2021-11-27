using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class EventLogger : MonoBehaviour {
  private void OnEnable() {
    BarelyMusician.OnInstrumentNoteOff += OnNoteOff;
    BarelyMusician.OnInstrumentNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    BarelyMusician.OnInstrumentNoteOff -= OnNoteOff;
    BarelyMusician.OnInstrumentNoteOn -= OnNoteOn;
  }

  private void OnNoteOff(Instrument instrument, float note_pitch) {
    Debug.Log(instrument.name + ": NoteOff(" + note_pitch + ")");
  }

  private void OnNoteOn(Instrument instrument, float note_pitch, float note_intensity) {
    Debug.Log(instrument.name + ": NoteOn(" + note_pitch + ", " + note_intensity + ")");
  }
}
