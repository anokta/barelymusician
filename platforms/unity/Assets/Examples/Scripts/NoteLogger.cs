using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class NoteLogger : MonoBehaviour {
  private Instrument instrument = null;

  private void OnEnable() {
    instrument = GetComponent<Instrument>();
    instrument.OnNoteOff += OnNoteOff;
    instrument.OnNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    instrument = GetComponent<Instrument>();
    instrument.OnNoteOff -= OnNoteOff;
    instrument.OnNoteOn -= OnNoteOn;
  }

  private void OnNoteOff(double pitch, double dspTime) {
    Debug.Log(instrument.name + ": NoteOff(" + pitch + ")");
  }

  private void OnNoteOn(double pitch, double intensity, double dspTime) {
    Debug.Log(instrument.name + ": NoteOn(" + pitch + ", " + intensity + ")");
  }
}
