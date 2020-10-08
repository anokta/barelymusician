using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SyncPlayback : MonoBehaviour {
  public AudioSource source;

  // Update is called once per frame
  void Update() {
    if (Input.GetKeyDown(KeyCode.J)) {
      source.Stop();
      BarelyApi.BarelyMusician.Stop();

      //source.pitch = (float)(tempo / 120.0);
      //BarelyApi.BarelyMusician.SetTempo(tempo);

      double dspTime = AudioSettings.dspTime;
      source.PlayScheduled(dspTime);
      BarelyApi.BarelyMusician.PlayScheduled(dspTime);
    } else if (source.isPlaying && Input.GetKeyDown(KeyCode.Space)) {
      source.Stop();
    }
  }
}