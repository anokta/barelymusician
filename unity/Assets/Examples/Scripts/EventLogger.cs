using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that logs every sequencer event.
public class EventLogger : MonoBehaviour {
  [Range(0.0f, 480.0f)]
  public double tempo = 120.0;

  void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
    BarelyMusician.OnNoteOff += OnNoteOff;
    BarelyMusician.OnNoteOn += OnNoteOn;
  }

  void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
    BarelyMusician.OnNoteOff -= OnNoteOff;
    BarelyMusician.OnNoteOn -= OnNoteOn;
  }

  void Update() {
    BarelyMusician.Instance.SetTempo(tempo);
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (BarelyMusician.Instance.IsPlaying()) {
        BarelyMusician.Instance.Pause();
      } else {
        BarelyMusician.Instance.Play();
      }
    } else if (Input.GetKeyDown(KeyCode.Backspace)) {
      BarelyMusician.Instance.Stop();
    } else if (Input.GetKeyDown(KeyCode.Return)) {
      Debug.Log("Position: " + BarelyMusician.Instance.GetPosition());
    }
  }

  private void OnBeat(int beat) {
    Debug.Log("Beat: " + beat);
  }

  private void OnNoteOff(Instrument instrument, float index) {
    Debug.Log(instrument.name + ": NoteOff(" + index + ")");
  }

  private void OnNoteOn(Instrument instrument, float index, float intensity) {
    Debug.Log(instrument.name + ": NoteOn(" + index + ", " + intensity + ")");
  }
}
