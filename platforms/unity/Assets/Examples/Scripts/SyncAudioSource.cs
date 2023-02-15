using UnityEngine;
using Barely;

public class SyncAudioSource : MonoBehaviour {
  public Metronome metronome;

  public AudioSource source;

  void Update() {
    if (Input.GetKeyDown(KeyCode.S)) {
      if (!source.isPlaying) {
        source.PlayScheduled(Musician.Timestamp);
        metronome.Stop();
        metronome.Play();
      } else {
        metronome.Stop();
        source.Stop();
      }
    }
  }
}
