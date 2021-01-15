using BarelyApi;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SyncPlayback : MonoBehaviour {
  public AudioSource source;

  // Update is called once per frame
  void Update() {
    if (Input.GetKeyDown(KeyCode.N)) {
      source.Stop();
      BarelyApi.BarelyMusician.Stop();

      source.pitch = (float)(BarelyMusician.GetTempo() / 120.0);

      double dspTime = AudioSettings.dspTime;
      source.PlayScheduled(dspTime);
      BarelyApi.BarelyMusician.PlayScheduled(dspTime);
    } else if (source.isPlaying && (Input.GetKeyDown(KeyCode.Space) ||
                                    Input.GetKeyDown(KeyCode.Backspace))) {
      source.Stop();
    }
  }
}
