using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class EventLogger : MonoBehaviour {
  private void OnEnable() {
    BarelyMusician.OnNoteOff += OnNoteOff;
    BarelyMusician.OnNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    BarelyMusician.OnNoteOff -= OnNoteOff;
    BarelyMusician.OnNoteOn -= OnNoteOn;
  }

  private void OnNoteOff(Instrument instrument, float pitch) {
    Debug.Log(instrument.name + ": NoteOff(" + pitch + ")");
  }

  private void OnNoteOn(Instrument instrument, float pitch, float intensity) {
    Debug.Log(instrument.name + ": NoteOn(" + pitch + ", " + intensity + ")");
  }
}
