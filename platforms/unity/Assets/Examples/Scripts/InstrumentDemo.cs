using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class InstrumentDemo : MonoBehaviour {
  public InstrumentController controller = null;
  public Texture2D pixel = null;
  public Color color = Color.white;

  private const int N = 4;
  private Dictionary<double, Vector2> _activePitches = null;
  private float[,] _alphas = null;
  private float[,] _targetAlphas = null;

  private void OnEnable() {
    _activePitches = new Dictionary<double, Vector2>();
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

  private void OnNoteOff(double pitch) {
    Vector2 value = Vector2.zero;
    if (_activePitches.TryGetValue(pitch, out value)) {
      _targetAlphas[(int)value.x, (int)value.y] = 0.0f;
      _activePitches.Remove(pitch);
    }
  }

  private void OnNoteOn(double pitch, double intensity) {
    int i = (int)((pitch - controller.octaveOffset - controller.rootPitch) * 12.0);
    int y = i / 4;
    int x = i - 4 * y;
    _targetAlphas[x, N - 1 - y] = 1.0f;
    _activePitches.Add(pitch, new Vector2(x, N - 1 - y));
  }
}
