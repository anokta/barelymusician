using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using UnityEngine;

namespace Barely {
  /// Pitch class.
  public enum PitchClass {
    [InspectorName("C")] C = 0,
    [InspectorName("C#")] CSHARP = 1,
    [InspectorName("D")] D = 2,
    [InspectorName("D#")] DSHARP = 3,
    [InspectorName("E")] E = 4,
    [InspectorName("F")] F = 5,
    [InspectorName("F#")] FSHARP = 6,
    [InspectorName("G")] G = 7,
    [InspectorName("G#")] GSHARP = 8,
    [InspectorName("A")] A = 9,
    [InspectorName("A#")] ASHARP = 10,
    [InspectorName("B")] B = 11,
    [InspectorName("Kick")] KICK = C,
    [InspectorName("Snare")] SNARE = D,
    [InspectorName("Hi-hat Closed")] HIHAT_CLOSED = E,
    [InspectorName("Hi-hat Open")] HIHAT_OPEN = F,
  }

  public enum ScaleType {
    [InspectorName("Chromatic")] CHROMATIC = 0,
    [InspectorName("Diatonic")] DIATONIC = 1,
    [InspectorName("Harmonic Minor")] HARMONIC_MINOR = 2,
    [InspectorName("Custom")] CUSTOM = -1,
  }

  [StructLayout(LayoutKind.Sequential)]
  struct ScaleDefinition {
    public IntPtr pitchRatios;
    public Int32 pitchRatioCount;
    public double rootNote;
  }

  /// A representation of a musical scale.
  [CreateAssetMenu(fileName = "NewScale", menuName = "BarelyMusician/Scale")]
  public class Scale : ScriptableObject {
    /// Scale type.
    public ScaleType type = ScaleType.CHROMATIC;

    /// Root note.
    public string rootNote = "";

    /// Returns the note for a given scale degree.
    ///
    /// @param degree Scale degree.
    /// @return Note value.
    public double GetNote(int degree) {
      double note = 0.0;
      if (!Barely_GetNoteFromScale(_definition, degree, ref note)) {
        Debug.LogError("Failed to get the scale note with a degree " + degree);
      }
      return note;
    }

    /// Returns the number of notes in the scale.
    ///
    /// @return Number of notes.
    public int GetNoteCount() {
      return _definition.pitchRatioCount;
    }

    private void Start() {
      OnValidate();
    }

    private void OnValidate() {
      if (!Barely_GetScaleDefinition(type, ParseRootNote(), ref _definition) &&
          Application.isPlaying) {
        Debug.LogError("Failed to parse the scale definition");
      }
    }

    private double ParseRootNote() {
      string rootNoteSanitized = rootNote.Trim().ToUpper();
      if (rootNoteSanitized.Length == 0) {
        return 0.0;
      }

      // Check for raw frequency.
      double rootNoteRaw = 0.0;
      if (double.TryParse(rootNoteSanitized,
                          NumberStyles.AllowDecimalPoint | NumberStyles.AllowThousands,
                          CultureInfo.InvariantCulture, out rootNoteRaw)) {
        return rootNoteRaw;
      }

      // Check for pitch.
      var match = _pitchRegex.Match(rootNoteSanitized);
      PitchClass pitchClass = PitchClass.C;
      switch (match.Groups[1].Value) {
        case "C":
          pitchClass = PitchClass.C;
          break;
        case "C#":
        case "DB":
          pitchClass = PitchClass.CSHARP;
          break;
        case "D":
          pitchClass = PitchClass.D;
          break;
        case "D#":
        case "EB":
          pitchClass = PitchClass.DSHARP;
          break;
        case "E":
          pitchClass = PitchClass.E;
          break;
        case "F":
          pitchClass = PitchClass.F;
          break;
        case "F#":
        case "GB":
          pitchClass = PitchClass.FSHARP;
          break;
        case "G":
          pitchClass = PitchClass.G;
          break;
        case "G#":
        case "AB":
          pitchClass = PitchClass.GSHARP;
          break;
        case "A":
          pitchClass = PitchClass.A;
          break;
        case "A#":
        case "BB":
          pitchClass = PitchClass.ASHARP;
          break;
        default:
          if (Application.isPlaying) {
            Debug.LogError("Failed to parse the root note pitch class " + rootNoteSanitized);
          }
          break;
      }
      int octave = match.Groups[2].Success ? int.Parse(match.Groups[2].Value) : 4;
      if (!Barely_GetNoteFromPitch(pitchClass, octave, ref rootNoteRaw)) {
        if (Application.isPlaying) {
          Debug.LogError("Failed to parse the root note pitch " + pitchClass.ToString() + octave);
        }
      }

      return rootNoteRaw;
    }

    // Definition.
    private ScaleDefinition _definition = new ScaleDefinition();

    // Pitch regex.
    private static readonly Regex _pitchRegex =
        new Regex(@"^([A-Ga-g][#b]?)([0-8])?$", RegexOptions.Compiled);

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "Barely_GetNoteFromPitch")]
    private static extern bool Barely_GetNoteFromPitch(PitchClass pitchClass, Int32 octave,
                                                       ref double outNote);

    [DllImport(pluginName, EntryPoint = "Barely_GetNoteFromScale")]
    private static extern bool Barely_GetNoteFromScale(ScaleDefinition definition, Int32 degree,
                                                       ref double outNote);

    [DllImport(pluginName, EntryPoint = "Barely_GetScaleDefinition")]
    private static extern bool Barely_GetScaleDefinition(ScaleType scaleType, double rootNote,
                                                         ref ScaleDefinition outDefinition);
  }
}  // namespace Barely
