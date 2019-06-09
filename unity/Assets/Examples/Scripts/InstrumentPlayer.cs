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
}
