using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class Conductor : MonoBehaviour {
  public enum ScaleType {
    Major,
    NaturalMinor,
    HarmonicMinor,
    Custom,
  }

  public readonly double[] MajorScale =
      new double[] { 0.0, 2.0 / 12.0, 4.0 / 12.0, 5.0 / 12.0, 7.0 / 12.0, 9.0 / 12.0, 11.0 / 12.0 };
  public readonly double[] NaturalMinorScale =
      new double[] { 0.0, 2.0 / 12.0, 3.0 / 12.0, 5.0 / 12.0, 7.0 / 12.0, 8.0 / 12.0, 10.0 / 12.0 };
  public readonly double[] HarmonicMinorScale =
      new double[] { 0.0, 2.0 / 12.0, 3.0 / 12.0, 5.0 / 12.0, 7.0 / 12.0, 8.0 / 12.0, 11.0 / 12.0 };

  public bool bypassAdjustment = false;

  [Range(-4.0f, 4.0f)]
  public double rootPitch = 0.0;

  public ScaleType scaleType = ScaleType.Major;

  public double[] scalePitches = null;

  void OnEnable() {
    Musician.OnAdjustNote += OnAdjustNote;
    UpdateScale();
  }

  void OnDisable() {
    Musician.OnAdjustNote -= OnAdjustNote;
  }

  void Update() {
    Musician.RootPitch = rootPitch;
    UpdateScale();
  }

  private void UpdateScale() {
    switch (scaleType) {
      case ScaleType.Major:
        scalePitches = MajorScale;
        break;
      case ScaleType.NaturalMinor:
        scalePitches = NaturalMinorScale;
        break;
      case ScaleType.HarmonicMinor:
        scalePitches = HarmonicMinorScale;
        break;
      case ScaleType.Custom:
        // Keep the scale as-is.
        break;
    }
    Musician.ScalePitches = scalePitches;
  }

  private void OnAdjustNote(ref NoteDefinition definition) {
    if (!bypassAdjustment) {
      definition.intensity *= 0.125 * (double)Random.Range(1, 8);
      definition.duration *= 0.125 * (double)Random.Range(1, 8);
    }
  }
}
