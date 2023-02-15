using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class Sparkler : MonoBehaviour {
  public Instrument targetInstrument;

  public Color noteOnColor = Color.white;

  private Color noteOffColor = Color.white;
  private Color targetColor = Color.white;

  private void OnEnable() {
    targetInstrument.OnNoteOff += OnNoteOff;
    targetInstrument.OnNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    targetInstrument.OnNoteOff -= OnNoteOff;
    targetInstrument.OnNoteOn -= OnNoteOn;
  }

  private void Start() {
    noteOffColor = GetComponent<Renderer>().material.color;
    targetColor = noteOffColor;
  }

  private void Update() {
    GetComponent<Renderer>().material.color =
        Color.Lerp(GetComponent<Renderer>().material.color, targetColor, 16 * Time.deltaTime);
  }

  private void OnNoteOff(double pitch) {
    targetColor = noteOffColor;
  }

  private void OnNoteOn(double pitch, double intensity) {
    targetColor = Color.Lerp(noteOffColor, noteOnColor, (float)intensity);
    GetComponent<Renderer>().material.color = targetColor;
  }
}
