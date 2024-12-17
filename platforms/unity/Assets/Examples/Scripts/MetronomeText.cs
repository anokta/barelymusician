using UnityEngine;

namespace Barely.Examples {
  public class MetronomeText : MonoBehaviour {
    public Metronome metronome;
    public TextMesh text;

    private void OnEnable() {
      metronome.OnBeat += OnBeat;
    }

    private void OnDisable() {
      metronome.OnBeat -= OnBeat;
    }

    private void OnBeat(int bar, int beat) {
      text.text = (bar + 1) + "." + (beat + 1);
    }

    private void Update() {
      if (Input.GetKeyDown(KeyCode.Backspace)) {
        text.text = "0.0";
      }
    }
  }
}  // namespace Barely.Examples
