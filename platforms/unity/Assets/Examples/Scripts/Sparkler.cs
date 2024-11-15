using UnityEngine;

namespace Barely {
  namespace Examples {
    public class Sparkler : MonoBehaviour {
      public Instrument targetInstrument;

      public Color noteOnColor = Color.white;

      private Color noteOffColor = Color.white;
      private Color targetColor = Color.white;

      private Renderer _renderer = null;

      private void Awake() {
        _renderer = GetComponent<Renderer>();
      }

      private void OnEnable() {
        targetInstrument.OnNoteOff += OnNoteOff;
        targetInstrument.OnNoteOn += OnNoteOn;
      }

      private void OnDisable() {
        targetInstrument.OnNoteOff -= OnNoteOff;
        targetInstrument.OnNoteOn -= OnNoteOn;
      }

      private void Start() {
        noteOffColor = _renderer.material.color;
        targetColor = noteOffColor;
      }

      private void Update() {
        _renderer.material.color =
            Color.Lerp(_renderer.material.color, targetColor, 8 * Time.deltaTime);
      }

      private void OnNoteOff(double pitch) {
        targetColor = noteOffColor;
      }

      private void OnNoteOn(double pitch, double intensity) {
        targetColor = Color.Lerp(noteOffColor, noteOnColor, (float)intensity);
        _renderer.material.color = targetColor;
      }
    }
  }  // namespace Examples
}  // namespace Barely
