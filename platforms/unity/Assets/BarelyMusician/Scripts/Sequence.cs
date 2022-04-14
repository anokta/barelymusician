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
    [Min(0.0f)]
    private double _endPosition = double.MaxValue;

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

    /// List of notes.
    public Note[] Notes {
      get { return _notes; }
      set {
        if (_notes != value) {
          if (_notes != null && Id != Musician.Native.InvalidId) {
            for (int i = 0; i < _notes.Length; ++i) {
              _notes[i].Destroy();
            }
          }
          _notes = value;
          if (_notes != null && Id != Musician.Native.InvalidId) {
            for (int i = 0; i < _notes.Length; ++i) {
              _notes[i].Create(this);
            }
          }
        }
      }
    }
    [SerializeField]
    private Note[] _notes = null;

    private void OnEnable() {
      Id = Musician.Native.Sequence_Create(this);
      Musician.Native.Sequence_SetBeginOffset(this, _beginOffset);
      _beginOffset = Musician.Native.Sequence_GetBeginOffset(this);
      Musician.Native.Sequence_SetBeginPosition(this, _beginPosition);
      _beginPosition = Musician.Native.Sequence_GetBeginPosition(this);
      Musician.Native.Sequence_SetEndPosition(this, _endPosition);
      _endPosition = Musician.Native.Sequence_GetEndPosition(this);
      Musician.Native.Sequence_SetLooping(this, _isLooping);
      _isLooping = Musician.Native.Sequence_IsLooping(this);
      Musician.Native.Sequence_SetLoopBeginOffset(this, _loopBeginOffset);
      _loopBeginOffset = Musician.Native.Sequence_GetLoopBeginOffset(this);
      Musician.Native.Sequence_SetLoopLength(this, _loopLength);
      _loopLength = Musician.Native.Sequence_GetLoopLength(this);
      Musician.Native.Sequence_SetInstrument(this, _instrument);
      _instrument = Musician.Native.Sequence_GetInstrument(this, _instrument);
      if (_notes != null) {
        for (int i = 0; i < _notes.Length; ++i) {
          _notes[i].Create(this);
        }
      }
    }

    private void OnDisable() {
      if (_notes != null) {
        for (int i = 0; i < _notes.Length; ++i) {
          _notes[i].Destroy();
        }
      }
      Musician.Native.Sequence_Destroy(this);
      Id = Musician.Native.InvalidId;
    }
  }
}
