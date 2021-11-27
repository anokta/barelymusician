using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace Barely {
  /// Musical note.
  [System.Serializable]
  public class Note {
    // TODO(#85): Should be float, keeping int for POC sequencer replacement.
    [Range(0, 128)]
    public int Pitch = 60;

    [Range(0.0f, 1.0f)]
    public float Intensity = 1.0f;

    public double Duration = 1.0;
  }
}
