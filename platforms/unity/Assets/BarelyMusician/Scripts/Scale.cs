using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// A representation of a musical scale.
  [CreateAssetMenu(fileName = "NewScale", menuName = "BarelyMusician/Scale")]
  public class Scale : ScriptableObject {
    /// Root note pitch.
    public double RootPitch = 0.0;

    /// Mode.
    [Min(0)]
    public int Mode = 0;

    /// Array of note pitches relative to the root note pitch.
    public double[] Pitches = null;

    /// Number of pitches.
    public int PitchCount {
      get { return (Pitches != null) ? Pitches.Length : 0; }
    }

    /// Returns the pitch for a given scale degree.
    ///
    /// @param degree Scale degree.
    /// @return Note pitch.
    public double GetPitch(int degree) {
      return Musician.Internal.Scale_GetPitch(this, degree);
    }
  }
}  // namespace Barely
