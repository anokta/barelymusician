using System.Runtime.InteropServices;
using System;
using UnityEngine;

namespace Barely {
  /// Musical note.
  [System.Serializable]
  public class Note {
    /// Pitch types.
    public enum PitchType {
      /// Absolute pitch.
      AbsolutePitch = 0,
      /// Relative pitch with respect to root note.
      RelativePitch = 1,
      /// Scale index with respect to root note and scale.
      ScaleIndex = 2,
    }

    /// Pitch definition.
    [StructLayout(LayoutKind.Sequential)]
    public struct PitchDefinition {
      /// Type.
      public PitchType type;

      /// Value.
      [StructLayout(LayoutKind.Explicit)]
      public struct Value {
        /// Absolute pitch.
        [FieldOffset(0)]
        public double absolutePitch;
        /// Relative pitch.
        [FieldOffset(0)]
        public double relativePitch;
        /// Scale index.
        [FieldOffset(0)]
        public int scaleIndex;
      };
      public Value value;
    }

    /// Definition.
    [StructLayout(LayoutKind.Sequential)]
    public struct Definition {
      /// Duration.
      public double duration;

      /// Pitch.
      public PitchDefinition pitch;

      /// Intensity.
      public double intensity;
    }

    /// Note identifier.
    public Int64 Id { get; private set; } = Musician.InvalidId;

    /// Constructs new `Note`.
    public Note(Sequence sequence, Definition definition, double position) {
      _sequence = sequence;
      Id = Musician.AddNote(sequence, definition, position);
    }

    ~Note() {
      Musician.RemoveNote(_sequence, this);
    }

    public void SetDefinition(Definition definition) {
      Musician.SetNoteDefinition(_sequence, this, definition);
    }

    public void SetPosition(double position) {
      Musician.SetNotePosition(_sequence, this, position);
    }

    // Sequence.
    private Sequence _sequence = null;
  }
}
