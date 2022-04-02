using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Barely;

public class Conductor : MonoBehaviour {
  void OnEnable() {
    Musician.OnAdjustNote += OnAdjustNote;
  }

  void OnDisable() {
    Musician.OnAdjustNote -= OnAdjustNote;
  }

  private void OnAdjustNote(ref Musician.NoteDefinition definition) {
    definition.intensity *= 0.25 * (double)Random.Range(1, 4);
    definition.duration *= 0.25 * (double)Random.Range(1, 4);
  }
}
