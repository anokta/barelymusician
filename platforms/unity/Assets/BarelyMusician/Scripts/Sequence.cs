using System;
using UnityEngine;

namespace Barely {
  /// Class that wraps sequence.
  public class Sequence : MonoBehaviour {
    /// Begin offset in beats.
    public double BeginOffset = 0.0;
    private double _beginOffset = 0.0;

    /// Begin position in beats.
    [Min(0.0f)]
    public double BeginPosition = 0.0;
    private double _beginPosition = 0.0;

    /// End position in bets.
    [Min(0.0f)]
    public double EndPosition = double.MaxValue;
    private double _endPosition = 0.0;

    /// Instrument.
    public Instrument Instrument = null;
    private Instrument _instrument = null;

    /// Denotes whether sequence is looping or not.
    public bool IsLooping;
    private bool _isLooping = false;

    /// Loop begin offset in beats.
    public double LoopBeginOffset = 0.0;
    private double _loopBeginOffset = 0.0;

    /// Loop length in beats.
    [Min(0.0f)]
    public double LoopLength = 1.0;
    private double _loopLength = 1.0;

    /// List of notes.
    public Note[] Notes = null;
    private Note[] _notes = null;

    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Native.InvalidId;

    private void OnEnable() {
      Id = Musician.Native.Sequence_Create(this);
      Musician.Native.Sequence_SetBeginOffset(this);
      _beginOffset = BeginOffset;
      Musician.Native.Sequence_SetBeginPosition(this);
      _beginPosition = BeginPosition;
      Musician.Native.Sequence_SetEndPosition(this);
      _endPosition = EndPosition;
      Musician.Native.Sequence_SetInstrument(this);
      _instrument = Instrument;
      Musician.Native.Sequence_SetLooping(this);
      _isLooping = IsLooping;
      Musician.Native.Sequence_SetLoopBeginOffset(this);
      _loopBeginOffset = LoopBeginOffset;
      Musician.Native.Sequence_SetLoopLength(this);
      _loopLength = LoopLength;
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

    private void Update() {
      if (_beginOffset != BeginOffset) {
        Musician.Native.Sequence_SetBeginOffset(this);
        _beginOffset = BeginOffset;
      }
      if (_beginPosition != BeginPosition) {
        Musician.Native.Sequence_SetBeginPosition(this);
        _beginPosition = BeginPosition;
      }
      if (_endPosition != EndPosition) {
        Musician.Native.Sequence_SetEndPosition(this);
        _endPosition = EndPosition;
      }
      if (_instrument != Instrument) {
        Musician.Native.Sequence_SetInstrument(this);
        _instrument = Instrument;
      }
      if (_isLooping != IsLooping) {
        Musician.Native.Sequence_SetLooping(this);
        _isLooping = IsLooping;
      }
      if (_loopBeginOffset != LoopBeginOffset) {
        Musician.Native.Sequence_SetLoopBeginOffset(this);
        _loopBeginOffset = LoopBeginOffset;
      }
      if (_loopLength != LoopLength) {
        Musician.Native.Sequence_SetLoopLength(this);
        _loopLength = LoopLength;
      }
      if (_notes != Notes) {
        if (_notes != null) {
          for (int i = 0; i < _notes.Length; ++i) {
            _notes[i].Destroy();
          }
        }
        _notes = Notes;
        if (_notes != null) {
          for (int i = 0; i < _notes.Length; ++i) {
            _notes[i].Create(this);
          }
        }
      } else {
        for (int i = 0; i < _notes.Length; ++i) {
          _notes[i].Update();
        }
      }
    }
  }
}
