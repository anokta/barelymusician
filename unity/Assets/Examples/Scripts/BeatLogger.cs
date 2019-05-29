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
          sequencer.Stop();
        } else {
          sequencer.Play();
        }
      }
    }

    public void OnBeat(int section, int bar, int beat) {
      Debug.Log("Beat: " + section + "." + bar + "." + beat);
    }
  }
}
