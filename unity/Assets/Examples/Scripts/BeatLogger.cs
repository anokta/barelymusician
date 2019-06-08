using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Example class that logs every sequencer beat.
  public class BeatLogger : MonoBehaviour {
    public Sequencer sequencer;

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
      Debug.Log(sequencer.Id + " Beat: " + section + "." + bar + "." + beat);
    }
  }
}
