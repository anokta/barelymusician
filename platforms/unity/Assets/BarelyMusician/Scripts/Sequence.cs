using System;
using UnityEngine;

namespace Barely {
  /// Sequence.
  public class Sequence : MonoBehaviour {
    /// Sequence id.
    public Int64 Id { get; private set; } = Musician.Api.InvalidId;

    /// Begin offset.
    public double BeginOffset = 0.0;

    /// Optional begin position.
    public double BeginPosition = 0.0;

    /// Optional end position.
    public double EndPosition = double.MaxValue;

    /// Denotes whether the sequence is looping or not.
    public bool Loop = false;

    /// Loop begin offset.
    public double LoopBeginOffset = 0.0;

    /// Loop length.
    public double LoopLength = 1.0;

    /// Instrument to perform.
    public Instrument Instrument = null;

    [Range(0, 128)]
    public int RootNote = 60;

    [System.Serializable]
    public class Note {
      // TODO(#85): Should be double, keeping int for POC sequencer replacement.
      [Range(0, 128)]
      public int Pitch = 60;
      [Range(0.0f, 1.0f)]
      public double Intensity = 1.0;
      public double Duration = 1.0;
    }

    /// Notes to perform.
    [System.Serializable]
    public struct SequenceNote {
      public double position;
      public Note note;
    }
    public SequenceNote[] Notes = null;
    public Barely.Note[] NativeNotes = null;

    private bool _changed = false;

    void OnEnable() {
      if (Id == Musician.Api.InvalidId) {
        Id = Musician.Api.Sequence_Create(this);
        _changed = true;
      }
    }

    void OnDisable() {
      if (Id != Musician.Api.InvalidId) {
        Musician.Api.Sequence_Destroy(this);
        Id = Musician.Api.InvalidId;
      }
    }

    void OnValidate() {
      _changed = true;
    }

    void Update() {
      Musician.Api.Sequence_Update(this, _changed);
      _changed = false;
    }
  }
}
