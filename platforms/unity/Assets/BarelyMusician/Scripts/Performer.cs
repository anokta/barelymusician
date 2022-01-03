using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// Performer.
  public class Performer : MonoBehaviour {
    /// Performer id.
    public Int64 Id { get; private set; } = Musician.InvalidId;

    /// Begin offset.
    public double BeginOffset = 0.0;

    /// Optional begin position.
    public double? BeginPosition = null;

    /// Optional end position.
    public double? EndPosition = null;

    /// Denotes whether the performer is looping or not.
    public bool Loop = false;

    /// Loop begin offset.
    public double LoopBeginOffset = 0.0;

    /// Loop length.
    public double LoopLength = 1.0;

    /// Instrument to perform.
    public Instrument Instrument = null;

    [Range(0, 128)]
    public int RootNote = 60;

    /// Notes to perform.
    [System.Serializable]
    public struct PerformerNote {
      public double position;
      public Note note;
    }
    public PerformerNote[] Notes = null;

    private bool _changed = false;

    void OnEnable() {
      if (Id == Musician.InvalidId) {
        Id = Musician.AddPerformer(this);
      }
    }

    void OnDisable() {
      if (Id != Musician.InvalidId) {
        Musician.RemovePerformer(this);
        Id = Musician.InvalidId;
      }
    }

    void OnValidate() {
      _changed = true;
    }

    void Update() {
      if (_changed) {
        _changed = false;
        Musician.UpdatePerformer(this);
      }
    }
  }
}
