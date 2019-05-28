using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Example class that logs every sequencer beat.
  public class BeatLogger : MonoBehaviour {
    void Awake() {
      BarelyMusician.Instance.RegisterSequencerBeatCallback(OnBeat);
    }

    public void OnBeat(int section, int bar, int beat) {
      Debug.Log("Beat: " + section + "." + bar + "." + beat);
    }
  }
}
