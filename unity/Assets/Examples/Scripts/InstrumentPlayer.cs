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
    isPlaying = false;
  }

  void OnEnable() {
    instrument.OnNoteOff += OnNoteOff;
    instrument.OnNoteOn += OnNoteOn;
  }

  void OnDisable() {
    instrument.OnNoteOff -= OnNoteOff;
    instrument.OnNoteOn -= OnNoteOn;
  }

  void Update() {
    if (Input.GetKeyDown(KeyCode.S)) {
      if (isPlaying) {
        instrument.StopNote(noteIndex);
      } else {
        instrument.StartNote(noteIndex, noteIntensity);
      }
      isPlaying = !isPlaying;
    }
  }

  private void OnNoteOff(float index) {
    Debug.Log("NoteOff(" + index + ")");
  }

  private void OnNoteOn(float index, float intensity) {
    Debug.Log("NoteOn(" + index + ", " + intensity + ")");
  }
}
