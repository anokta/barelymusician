using System;
using UnityEngine;

namespace Barely {
  /// Class that wraps sequence.
  public class Sequence : MonoBehaviour {
    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Native.InvalidId;

    /// Begin offset in beats.
    public double BeginOffset {
      get { return _beginOffset; }
      set {
        if (_beginOffset != value) {
          Musician.Native.Sequence_SetBeginOffset(this, value);
          _beginOffset = Musician.Native.Sequence_GetBeginOffset(this);
        }
      }
    }
    [SerializeField]
    private double _beginOffset = 0.0;

    /// Begin position in beats.
    public double BeginPosition {
      get { return _beginPosition; }
      set {
        if (_beginPosition != value) {
          Musician.Native.Sequence_SetBeginPosition(this, value);
          _beginPosition = Musician.Native.Sequence_GetBeginPosition(this);
        }
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _beginPosition = 0.0;

    /// End position in bets.
    public double EndPosition {
      get { return _endPosition; }
      set {
        if (_endPosition != value) {
          Musician.Native.Sequence_SetEndPosition(this, value);
          _endPosition = Musician.Native.Sequence_GetEndPosition(this);
        }
      }
    }
    [SerializeField]
    private double _endPosition = double.MaxValue;

    /// Denotes whether sequence is looping or not.
    public bool IsLooping {
      get { return _isLooping; }
      set {
        if (_isLooping != value) {
          Musician.Native.Sequence_SetLooping(this, value);
          _isLooping = Musician.Native.Sequence_IsLooping(this);
        }
      }
    }
    [SerializeField]
    private bool _isLooping = false;

    /// Loop begin offset in beats.
    public double LoopBeginOffset {
      get { return _loopBeginOffset; }
      set {
        if (_loopBeginOffset != value) {
          Musician.Native.Sequence_SetLoopBeginOffset(this, value);
          _loopBeginOffset = Musician.Native.Sequence_GetLoopBeginOffset(this);
        }
      }
    }
    [SerializeField]
    private double _loopBeginOffset = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        if (_loopLength != value) {
          Musician.Native.Sequence_SetLoopLength(this, value);
          _loopLength = Musician.Native.Sequence_GetLoopLength(this);
        }
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// Instrument.
    public Instrument Instrument {
      get { return _instrument; }
      set {
        if (_instrument != value) {
          Musician.Native.Sequence_SetInstrument(this, value);
          _instrument = Musician.Native.Sequence_GetInstrument(this, value);
        }
      }
    }
    [SerializeField]
    private Instrument _instrument = null;

    [Serializable]
    public class Note {
      // TODO(#85): Should be double, keeping int for POC sequencer replacement.
      [Range(-24, 24)]
      public int Pitch = 0;
      [Range(0.0f, 1.0f)]
      public double Intensity = 1.0;
      public double Duration = 1.0;
    }

    /// Notes to perform.
    [Serializable]
    public struct SequenceNote {
      public double position;
      public Note note;
    }
    public SequenceNote[] Notes = null;
    public Barely.Note[] NativeNotes = null;

    private bool _changed = false;

    void OnEnable() {
      if (Id == Musician.Native.InvalidId) {
        Id = Musician.Native.Sequence_Create(this);
        _changed = true;
      }
      if (NativeNotes != null) {
        for (int i = 0; i < NativeNotes.Length; ++i) {
          NativeNotes[i].Create(this);
        }
      }
    }

    void OnDisable() {
      if (NativeNotes != null) {
        for (int i = 0; i < NativeNotes.Length; ++i) {
          NativeNotes[i].Destroy();
        }
      }
      if (Id != Musician.Native.InvalidId) {
        Musician.Native.Sequence_Destroy(this);
        Id = Musician.Native.InvalidId;
      }
    }

    void OnValidate() {
      _changed = true;
    }

    void Update() {
      Musician.Native.Sequence_Update(this, _changed);
      _changed = false;
    }
  }
}
