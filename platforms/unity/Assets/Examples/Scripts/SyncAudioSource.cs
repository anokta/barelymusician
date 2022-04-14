using UnityEngine;
using Barely;

public class SyncAudioSource : MonoBehaviour {
  public AudioSource source;

  void Update() {
    if (Input.GetKeyDown(KeyCode.S)) {
      if (!source.isPlaying) {
        double dspTime = AudioSettings.dspTime;
        Musician.PlayScheduled(dspTime);
        source.PlayScheduled(dspTime);
      } else {
        Musician.Stop();
        source.Stop();
      }
    }
  }
}
