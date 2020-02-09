using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that logs every sequencer event.
public class EventLogger : MonoBehaviour {
  private Sequencer sequencer = null;

  void Awake() {
    sequencer = GetComponent<Sequencer>();
  }

  void OnDestroy() {
    sequencer = null;
  }

  void OnEnable() {
    sequencer.OnBeat += OnBeat;
    sequencer.OnNoteOff += OnNoteOff;
    sequencer.OnNoteOn += OnNoteOn;
  }

  void OnDisable() {
    sequencer.OnBeat -= OnBeat;
    sequencer.OnNoteOff -= OnNoteOff;
    sequencer.OnNoteOn -= OnNoteOn;
  }

  void Update() {
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (sequencer.IsPlaying) {
        sequencer.Pause();
      } else {
        sequencer.Play();
      }
    } else if (Input.GetKeyDown(KeyCode.Backspace)) {
      sequencer.Stop();
    } else if (Input.GetKeyDown(KeyCode.Return)) {
      Debug.Log("Position: " + sequencer.GetPosition());
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
