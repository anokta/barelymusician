using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  namespace Examples {
    public class InstrumentDemo : MonoBehaviour {
      public InstrumentController controller = null;
      public Texture2D pixel = null;
      public Color color = Color.white;

      public Arpeggiator arpeggiator = null;
      public bool enableModeSwitch = false;
      public OscillatorShape oscillatorShape = OscillatorShape.SINE;
      public SamplePlaybackMode samplePlaybackMode = SamplePlaybackMode.LOOP;

      private const int N = 4;
      private Dictionary<double, Vector2> _activeNotes = null;
      private float[,] _alphas = null;
      private float[,] _targetAlphas = null;

      private void Awake() {
        if (enableModeSwitch) {
          controller.instrument.OscillatorShape = oscillatorShape;
          controller.instrument.SamplePlaybackMode = SamplePlaybackMode.NONE;
        }
      }

      private void OnEnable() {
        _activeNotes = new Dictionary<double, Vector2>();
        _alphas = new float[N, N];
        _targetAlphas = new float[N, N];
        controller.instrument.OnNoteOff += OnNoteOff;
        controller.instrument.OnNoteOn += OnNoteOn;
      }

      private void OnDisable() {
        _alphas = null;
        _targetAlphas = null;
        controller.instrument.OnNoteOff -= OnNoteOff;
        controller.instrument.OnNoteOn -= OnNoteOn;
      }

      private void OnGUI() {
        GUI.depth = -100;
        for (int x = 0; x < N; ++x) {
          for (int y = 0; y < N; ++y) {
            _alphas[x, y] = Mathf.Lerp(_alphas[x, y], _targetAlphas[x, y], 8.0f * Time.deltaTime);
            color.a = _alphas[x, y];
            GUI.color = color;
            GUI.DrawTexture(new Rect(x * Screen.width / N, y * Screen.height / N, Screen.width / N,
                                     Screen.height / N),
                            pixel);
          }
        }
      }

      private void Update() {
        if (!enableModeSwitch) {
          return;
        }
        if (Input.GetKeyDown(KeyCode.Alpha1) || Input.GetKeyDown(KeyCode.Keypad1)) {
          controller.instrument.OscillatorShape = oscillatorShape;
          controller.instrument.SamplePlaybackMode = SamplePlaybackMode.NONE;
        } else if (Input.GetKeyDown(KeyCode.Alpha2) || Input.GetKeyDown(KeyCode.Keypad2)) {
          controller.instrument.OscillatorShape = OscillatorShape.NONE;
          controller.instrument.SamplePlaybackMode = samplePlaybackMode;
        } else if (Input.GetKeyDown(KeyCode.Alpha3) || Input.GetKeyDown(KeyCode.Keypad3)) {
          controller.instrument.OscillatorShape = oscillatorShape;
          controller.instrument.SamplePlaybackMode = samplePlaybackMode;
        } else if (Input.GetKeyDown(KeyCode.Alpha0) || Input.GetKeyDown(KeyCode.Keypad0)) {
          if (controller.arpeggiator != null) {
            controller.arpeggiator.SetAllNotesOff();
            controller.arpeggiator = null;
          } else {
            controller.instrument.SetAllNotesOff();
            controller.arpeggiator = arpeggiator;
          }
        }
      }

      private void OnNoteOff(double pitch) {
        Vector2 value = Vector2.zero;
        if (_activeNotes.TryGetValue(pitch, out value)) {
          _targetAlphas[(int)value.x, (int)value.y] = 0.0f;
          _activeNotes.Remove(pitch);
        }
      }

      private void OnNoteOn(double pitch, double intensity) {
        int i = (int)(pitch * 12.0);
        int y = i / 4;
        int x = i - 4 * y;
        if (x < 0 || x >= N || y < 0 || y >= N) {
          return;
        }
        _targetAlphas[x, N - 1 - y] = 1.0f;
        _activeNotes.Add(pitch, new Vector2(x, N - 1 - y));
      }
    }
  }  // namespace Examples
}  // namespace Barely
