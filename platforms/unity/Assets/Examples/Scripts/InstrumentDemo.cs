using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class InstrumentDemo : MonoBehaviour {
  public InstrumentController controller = null;
  public Texture2D pixel = null;

  private const int N = 4;
  private float[,] _alphas = null;
  private float[,] _targetAlphas = null;

  private void OnEnable() {
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
    Color color = Color.white;
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

  private void OnNoteOff(double pitch, double dspTime) {
    int i = (int)((pitch - controller.octaveOffset - controller.rootPitch) * 12.0);
    int y = i / 4;
    int x = i - 4 * y;
    _targetAlphas[x, N - 1 - y] = 0.0f;
  }

  private void OnNoteOn(double pitch, double intensity, double dspTime) {
    int i = (int)(pitch * 12.0);
    int y = i / 4;
    int x = i - 4 * y;
    _targetAlphas[x, N - 1 - y] = 1.0f;
  }
}
