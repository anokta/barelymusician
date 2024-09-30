using UnityEngine;

namespace Barely {
  namespace Examples {
    public class SequencerDemo : MonoBehaviour {
      public Sequencer randomSequencer;
      public Scale scale;

      public void Randomize() {
        randomSequencer.notes[0].pitch = scale.GetPitch(Random.Range(0, scale.PitchCount));
        randomSequencer.notes[0].intensity = 0.5 * (double)Random.Range(0, 3);
        // randomSequencer.notes[0].position = (double)Random.Range(0, 2) * 0.5;
        randomSequencer.notes[0].duration = 0.125f + (double)Random.Range(1, 3) * 0.125;
      }

      private void Update() {
        if (((Application.platform == RuntimePlatform.Android ||
              Application.platform == RuntimePlatform.IPhonePlayer) &&
             Input.GetMouseButtonDown(0)) ||
            Input.GetKeyDown(KeyCode.Space)) {
          foreach (var sequencer in GameObject.FindObjectsOfType<Sequencer>()) {
            if (sequencer.IsPlaying) {
              sequencer.Pause();
            } else {
              sequencer.Play();
            }
          }
        } else if (Input.GetKeyDown(KeyCode.Backspace)) {
          foreach (var sequencer in GameObject.FindObjectsOfType<Sequencer>()) {
            sequencer.Stop();
          }
        }
      }
    }
  }  // namespace Examples
}  // namespace Barely
