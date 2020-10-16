using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

public class Teleporter : MonoBehaviour {
  public Vector3[] locations;

  private void Awake() {
    BarelyMusician.OnBeat += OnBeat;
  }

  private void OnDestroy() {
    BarelyMusician.OnBeat -= OnBeat;
  }

  private void Update() {
    if (!BarelyMusician.IsPlaying()) {
      gameObject.SetActive(false);
    }
  }

  private void OnBeat(double dspTime, int beat) {
    transform.localPosition = locations[beat % locations.Length];
    gameObject.SetActive(true);
  }
}
