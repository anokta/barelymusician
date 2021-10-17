using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Clock : MonoBehaviour {
  [Range(30.0f, 480.0f)]
  public double tempo = 120.0;

  private double dspTime = 0.0;
  private double position = 0.0;

  private Sequencer[] sequencers = null;

  void Start() {
    dspTime = AudioSettings.dspTime + 1.0;
  }

  void Update() {
    sequencers = GameObject.FindObjectsOfType<Sequencer>();
    double lookahead = 2.0 * (double)Time.smoothDeltaTime;
    double newDspTime = AudioSettings.dspTime + lookahead;
    if (newDspTime <= dspTime) return;
    double newPosition = position + (tempo / 60.0) * (newDspTime - dspTime);
    for (double beat = System.Math.Ceiling(position); beat < newPosition; ++beat) {
      foreach (var sequencer in sequencers) {
        sequencer.OnBeat(beat);
      }
    }
    dspTime = newDspTime;
    position = newPosition;
  }

  public double GetTimestamp(double targetPosition) {
    return dspTime + (targetPosition - position) / (tempo / 60.0);
  }
}
