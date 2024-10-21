using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  public enum ScaleType {
    [InspectorName("Chromatic")] CHROMATIC = 0,
    [InspectorName("Diatonic")] DIATONIC = 1,
    [InspectorName("Harmonic Minor")] HARMONIC_MINOR = 2,
    [InspectorName("Custom")] CUSTOM = -1,
  }

  /// A representation of a musical scale.
  [CreateAssetMenu(fileName = "NewScale", menuName = "BarelyMusician/Scale")]
  public class Scale : ScriptableObject {
    /// Scale type.
    public ScaleType type = ScaleType.CHROMATIC;

    /// Root pitch.
    public double rootPitch = 0.0;

    /// Mode.
    [Min(0)]
    public int mode = 0;

    /// Number of pitches.
    public int PitchCount {
      get { return _scale.pitchCount; }
    }

    /// Returns the pitch for a given scale degree.
    ///
    /// @param degree Scale degree.
    /// @return Note pitch.
    public double GetPitch(int degree) {
      double pitch = 0.0;
      if (!BarelyScale_GetPitch(ref _scale, degree, ref pitch)) {
        Debug.LogError("Failed to get the scale note with a degree " + degree);
      }
      return pitch;
    }

    private void Start() {
      OnValidate();
    }

    private void OnValidate() {
      if (!Barely_GetScale(type, rootPitch, ref _scale) && Application.isPlaying) {
        Debug.LogError("Failed to parse the scale");
      }
    }

    /// Class that wraps the internal api.
    public static class Internal {
      [StructLayout(LayoutKind.Sequential)]
      public struct Scale {
        public IntPtr pitches;
        public Int32 pitchCount;
        public double rootPitch;
        public Int32 mode;
      }
    }
    private Internal.Scale _scale = new Internal.Scale();

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "BarelyScale_GetPitch")]
    private static extern bool BarelyScale_GetPitch(ref Internal.Scale scale, Int32 degree,
                                                    ref double outPitch);

    [DllImport(pluginName, EntryPoint = "Barely_GetScale")]
    private static extern bool Barely_GetScale(ScaleType scaleType, double rootPitch,
                                               ref Internal.Scale outScale);
  }
}  // namespace Barely
