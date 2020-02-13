using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Pulser : MonoBehaviour {
  public Transform targetObject;

  public Color noteOffColor = Color.white;

  private Vector3 targetScale = Vector3.one;
  private Color targetColor = Color.white;


  void Awake() {
    targetScale = targetObject.localScale;
  }

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
    targetObject.localScale = 
        Vector3.Lerp(targetObject.localScale, targetScale, Time.deltaTime * 4);
    targetObject.GetComponent<Renderer>().material.color = 
        Color.Lerp(targetObject.GetComponent<Renderer>().material.color, targetColor, 
                   64 * Time.deltaTime);
  }

  private void OnBeat(int beat) {
    targetObject.localScale = targetScale * 2.0f;
  }

  private void OnNoteOff(Instrument instrument, float index) {
    targetColor = noteOffColor;
  }

  private void OnNoteOn(Instrument instrument, float index, float intensity) {
    targetColor = Random.ColorHSV(0.0f, 1.0f, 0.5f, 1.0f, 0.75f, 1.0f);
  }
}
