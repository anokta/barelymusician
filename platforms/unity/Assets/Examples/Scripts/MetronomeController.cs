using UnityEngine;
using Barely;

public class MetronomeController : MonoBehaviour {
  public Metronome metronome;

  private void Update() {
    if (Input.GetKeyDown(KeyCode.Space)) {
      if (metronome.IsPlaying) {
        metronome.Pause();
      } else {
        metronome.Play();
      }
    } else if (Input.GetKeyDown(KeyCode.Backspace)) {
      metronome.Stop();
    }
  }
}
