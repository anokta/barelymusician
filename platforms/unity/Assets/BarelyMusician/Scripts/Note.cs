using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Note pitch type.
  public enum NotePitchType {
    /// Absolute pitch.
    [InspectorName("Absolute Pitch")] ABSOLUTE_PITCH = 0,
    /// Relative pitch with respect to root note.
    [InspectorName("Relative Pitch")] RELATIVE_PITCH = 1,
    /// Scale index with respect to root note and scale.
    [InspectorName("Scale Index")] SCALE_INDEX = 2,
  }

  /// Note pitch value.
  [Serializable]
  [StructLayout(LayoutKind.Explicit)]
  public struct NotePitchValue {
    /// Absolute pitch.
    [FieldOffset(0)]
    public double AbsolutePitch;
    /// Relative pitch.
    [FieldOffset(0)]
    public double RelativePitch;
    /// Scale index.
    [FieldOffset(0)]
    public int ScaleIndex;
  };

  /// Note pitch definition.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct NotePitchDefinition {
    /// Type.
    public NotePitchType Type;

    /// Value.
    public NotePitchValue Value;
  }

  /// Note definition.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct NoteDefinition {
    /// Duration.
    [Min(0.0f)]
    public double Duration;

    /// Pitch.
    public NotePitchDefinition Pitch;

    /// Intensity.
    [Range(0.0f, 1.0f)]
    public double Intensity;
  }

  /// Class that wraps note.
  [Serializable]
  public class Note {
    /// Definition.
    public NoteDefinition Definition = default(NoteDefinition);
    private NoteDefinition _definition = default(NoteDefinition);

    /// Position in beats.
    [Min(0.0f)]
    public double Position = 0.0;
    private double _position = 0.0;

    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Native.InvalidId;

    /// Sequence.
    public Sequence Sequence { get; private set; }

    /// Creates native note.
    ///
    /// @param sequence Sequence.
    public void Create(Sequence sequence) {
      Sequence = sequence;
      Id = Musician.Native.Note_Create(this);
      _definition = Definition;
      _position = Position;
    }

    /// Destroys native note.
    public void Destroy() {
      Musician.Native.Note_Destroy(this);
      Id = Musician.Native.InvalidId;
      Sequence = null;
    }

    /// Updates native note.
    public void Update() {
      if (!_definition.Equals(Definition)) {
        Musician.Native.Note_SetDefinition(this);
        _definition = Definition;
      }
      if (_position != Position) {
        Musician.Native.Note_SetPosition(this);
        _position = Position;
      }
    }
  }
}
