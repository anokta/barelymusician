using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Note pitch types.
  public enum NotePitchType {
    /// Absolute pitch.
    [InspectorName("Absolute Pitch")] ABSOLUTE_PITCH = 0,
    /// Relative pitch with respect to root note.
    [InspectorName("Relative Pitch")] RELATIVE_PITCH = 1,
    /// Scale index with respect to root note and scale.
    [InspectorName("Scale Index")] SCALE_INDEX = 2,
  }

  /// Note pitch definition.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct NotePitchDefinition {
    /// Absolute pitch.
    public double AbsolutePitch {
      get { return (Type == NotePitchType.ABSOLUTE_PITCH) ? _value.AbsolutePitch : 0.0; }
      set {
        _type = NotePitchType.ABSOLUTE_PITCH;
        _value.AbsolutePitch = value;
      }
    }

    /// Relative pitch.
    public double RelativePitch {
      get { return (Type == NotePitchType.RELATIVE_PITCH) ? _value.RelativePitch : 0.0; }
      set {
        _type = NotePitchType.RELATIVE_PITCH;
        _value.RelativePitch = value;
      }
    }

    /// Scale index.
    public int ScaleIndex {
      get { return (Type == NotePitchType.SCALE_INDEX) ? _value.ScaleIndex : 0; }
      set {
        _type = NotePitchType.SCALE_INDEX;
        _value.ScaleIndex = value;
      }
    }

    /// Type.
    public NotePitchType Type {
      get { return _type; }
    }
    [SerializeField]
    private NotePitchType _type;

    /// Value.
    [Serializable]
    [StructLayout(LayoutKind.Explicit)]
    private struct Value {
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
    [SerializeField]
    private Value _value;
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
    public NoteDefinition Definition {
      get { return _definition; }
      set {
        if (!_definition.Equals(value)) {
          Musician.Native.Note_SetDefinition(this, value);
          _definition = Musician.Native.Note_GetDefinition(this);
        }
      }
    }
    [SerializeField]
    private NoteDefinition _definition = new NoteDefinition();

    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Native.InvalidId;

    /// Position in beats.
    public double Position {
      get { return _position; }
      set {
        if (_position != value) {
          Musician.Native.Note_SetPosition(this, value);
          _position = Musician.Native.Note_GetPosition(this);
        }
      }
    }
    [SerializeField]
    private double _position = 0.0;

    /// Sequence.
    public Sequence Sequence { get; private set; }

    /// Creates native note.
    ///
    /// @param sequence Sequence.
    public void Create(Sequence sequence) {
      Sequence = sequence;
      Id = Musician.Native.Note_Create(this);
    }

    /// Destroys native note.
    public void Destroy() {
      Musician.Native.Note_Destroy(this);
      Id = Musician.Native.InvalidId;
      Sequence = null;
    }
  }
}
