using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that logs every sequencer event.
public class EventLogger : MonoBehaviour {
  [Range(0.0f, 960.0f)]
  public double tempo = 120.0;

  private void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
    BarelyMusician.OnNoteOff += OnNoteOff;
    BarelyMusician.OnNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
    BarelyMusician.OnNoteOff -= OnNoteOff;
    BarelyMusician.OnNoteOn -= OnNoteOn;
  }

  private void Update() {
    BarelyMusician.SetTempo(tempo);
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (BarelyMusician.IsPlaying()) {
        BarelyMusician.Pause();
      } else {
        BarelyMusician.Play();
      }
    } else if (Input.GetKeyDown(KeyCode.Backspace)) {
      BarelyMusician.Stop();
    } else if (Input.GetKeyDown(KeyCode.Return)) {
      Debug.Log("Position: " + BarelyMusician.GetPosition());
    }
  }

  private void OnBeat(double dspTime,  int beat) {
    //Debug.Log("Beat: " + beat);
  }

  private void OnNoteOff(double dspTime, Instrument instrument, float pitch) {
    Debug.Log(instrument.name + ": NoteOff(" + pitch + ")");
  }

  private void OnNoteOn(double dspTime, Instrument instrument, float pitch, float intensity) {
    Debug.Log(instrument.name + ": NoteOn(" + pitch + ", " + intensity + ")");
  }
}
