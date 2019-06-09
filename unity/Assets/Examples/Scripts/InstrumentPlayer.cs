using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that plays instrument.
public class InstrumentPlayer : MonoBehaviour {
  // Instrument to play.
  private SineInstrument instrument = null;

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
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (isPlaying) {
        instrument.Stop();
      } else {
        instrument.Play();
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
