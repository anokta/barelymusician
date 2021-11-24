using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Sparkler : MonoBehaviour {
  public Instrument targetInstrument;

  public Color noteOffColor = Color.white;

  private Color targetColor = Color.white;

  private void OnEnable() {
    BarelyMusician.OnInstrumentNoteOff += OnNoteOff;
    BarelyMusician.OnInstrumentNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    BarelyMusician.OnInstrumentNoteOff -= OnNoteOff;
    BarelyMusician.OnInstrumentNoteOn -= OnNoteOn;
  }

  private void Start() {
    targetColor = noteOffColor;
  }

  private void Update() {
    transform.GetComponent<Renderer>().material.color =
        Color.Lerp(transform.GetComponent<Renderer>().material.color, targetColor, 32 * Time.deltaTime);
  }

  private void OnNoteOff(Instrument instrument, float note_pitch) {
    if (instrument != targetInstrument) return;
    targetColor = noteOffColor;
  }

  private void OnNoteOn(Instrument instrument, float note_pitch, float note_intensity) {
    if (instrument != targetInstrument) return;
    targetColor = Random.ColorHSV(0.0f, 1.0f, 1.0f, 1.0f, note_intensity, note_intensity);
  }
}
