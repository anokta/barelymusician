using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  /// Performer.
  public class Performer : MonoBehaviour {
    /// Performer id.
    public Int64 Id { get; private set; } = BarelyMusician.InvalidId;

    /// Begin offset.
    public double BeginOffset = 0.0;

    /// Optional begin position.
    public double? BeginPosition = null;

    /// Optional end position.
    public double? EndPosition = null;

    /// Denotes whether looping or not.
    public bool IsLooping = false;

    /// Loop begin offset.
    public double LoopBeginOffset = 0.0;

    /// Loop length.
    public double LoopLength = 1.0;

    /// Instruments to perform.
    public Instrument[] Instruments = null;

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
      if (Id == BarelyMusician.InvalidId) {
        Id = BarelyMusician.AddPerformer(this);
      }
    }

    void OnDisable() {
      if (Id != BarelyMusician.InvalidId) {
        BarelyMusician.RemovePerformer(this);
        Id = BarelyMusician.InvalidId;
      }
    }

    void OnValidate() {
      _changed = true;
    }

    void Update() {
      if (_changed) {
        _changed = false;
        BarelyMusician.UpdatePerformer(this);
      }
    }
  }
}
