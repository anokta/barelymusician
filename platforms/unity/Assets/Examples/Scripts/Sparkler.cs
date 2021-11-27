using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class Sparkler : MonoBehaviour {
  public Instrument targetInstrument;

  public Color noteOffColor = Color.white;

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
    targetColor = noteOffColor;
  }

  private void Update() {
    transform.GetComponent<Renderer>().material.color = Color.Lerp(transform.GetComponent<Renderer>().material.color,
                                                                   targetColor, 32 * Time.deltaTime);
  }

  private void OnNoteOff(float pitch) {
    targetColor = noteOffColor;
  }

  private void OnNoteOn(float pitch, float intensity) {
    targetColor = Random.ColorHSV(0.0f, 1.0f, 1.0f, 1.0f, intensity, intensity);
  }
}
