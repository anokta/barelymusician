using UnityEngine;

namespace Barely {
  namespace Examples {
    public class SequencerDemo : MonoBehaviour {
      public Sequencer randomSequencer;

      private readonly int[] Scale = new int[] { 0, 4, 5, 7, 9, 12 };

      public void Randomize() {
        randomSequencer.notes[0].key = 57 + Scale[Random.Range(0, Scale.Length)];
        randomSequencer.notes[0].intensity = 0.5f * Random.Range(0, 3);
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
