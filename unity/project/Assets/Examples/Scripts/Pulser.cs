using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Pulser : MonoBehaviour {
  public float scaleMultiplier = 2.0f;

  private Vector3 targetScale = Vector3.one;

  private void Awake() {
    targetScale = transform.localScale;
  }

  private void OnEnable() {
    BarelyMusician.OnBeat += OnBeat;
  }

  private void OnDisable() {
    BarelyMusician.OnBeat -= OnBeat;
  }

  private void Update() {
    float speed = (float)(BarelyMusician.GetTempo() / 60.0);
    transform.localScale = Vector3.Lerp(transform.localScale, targetScale, Time.deltaTime * speed);
  }

  private void OnBeat(int beat) {
    transform.localScale = targetScale * scaleMultiplier;
  }
}
