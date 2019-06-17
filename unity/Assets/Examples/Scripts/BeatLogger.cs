using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using BarelyApi;

// Example class that logs every sequencer beat.
public class BeatLogger : MonoBehaviour {
  private Sequencer sequencer = null;

  void Awake() {
    sequencer = GetComponent<Sequencer>();
  }

  void OnDestroy() {
    sequencer = null;
  }

  void OnEnable() {
    sequencer.OnBeat += OnBeat;
  }

  void OnDisable() {
    sequencer.OnBeat -= OnBeat;
  }

  void Update() {
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (sequencer.IsPlaying) {
        sequencer.Pause();
      } else {
        sequencer.Play();
      }
    }
    if (Input.GetKeyDown(KeyCode.Backspace)) {
      sequencer.Stop();
    }
  }

  public void OnBeat(int section, int bar, int beat) {
    Debug.Log("Tick: " + section + "." + bar + "." + beat);
  }
}
