using UnityEngine;

namespace Barely.Examples {
  public class SequencerDemo : MonoBehaviour {
    public Sequencer randomSequencer;
    public Scale scale;

    public void Randomize() {
      randomSequencer.notes[0].pitch =
          (int)System.Math.Round(12.0 * (double)scale.GetPitch(Random.Range(0, scale.PitchCount)));
      randomSequencer.notes[0].gain = 0.5 * Random.Range(0, 3);
      randomSequencer.notes[0].position = 0.0;
      randomSequencer.notes[0].duration = 0.125 * Random.Range(2, 4);
      if (randomSequencer.notes[0].gain > 0.0) {
        randomSequencer.instrument.StereoPan = (double)Random.Range(-0.5f, 0.5f);
      }
    }

    private void Update() {
      if (((Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer) &&
           Input.GetMouseButtonDown(0)) ||
          Input.GetKeyDown(KeyCode.Space)) {
        foreach (var sequencer in GameObject.FindObjectsByType<Sequencer>(
                     FindObjectsSortMode.None)) {
          if (sequencer.IsPlaying) {
            sequencer.Pause();
          } else {
            sequencer.Play();
          }
        }
      } else if (Input.GetKeyDown(KeyCode.Backspace)) {
        foreach (var sequencer in GameObject.FindObjectsByType<Sequencer>(
                     FindObjectsSortMode.None)) {
          sequencer.Stop();
        }
      }
    }
  }
}  // namespace Barely.Examples
