using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Sparkler : MonoBehaviour {
  public Color noteOffColor = Color.white;

  private Color targetColor = Color.white;

  private void OnEnable() {
    BarelyMusician.OnNoteOff += OnNoteOff;
    BarelyMusician.OnNoteOn += OnNoteOn;
  }

  private void OnDisable() {
    BarelyMusician.OnNoteOff -= OnNoteOff;
    BarelyMusician.OnNoteOn -= OnNoteOn;
  }

  private void Start() {
    targetColor = noteOffColor;
  }

  private void Update() {
    transform.GetComponent<Renderer>().material.color =
        Color.Lerp(transform.GetComponent<Renderer>().material.color, targetColor,
                   32 * Time.deltaTime);
  }

  private void OnNoteOff(double dspTime, Instrument instrument, float index) {
    targetColor = noteOffColor;
  }

  private void OnNoteOn(double dspTime, Instrument instrument, float index, float intensity) {
    targetColor = Random.ColorHSV(0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  }
}
