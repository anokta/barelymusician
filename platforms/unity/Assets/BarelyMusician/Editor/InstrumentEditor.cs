using UnityEditor;
using UnityEngine;

namespace Barely {
  [CustomEditor(typeof(Instrument))]
  public class InstrumentEditor : Editor {
    public override void OnInspectorGUI() {
      base.OnInspectorGUI();

      // TODO(#161): Support preview in Edit mode as well.
      if (Application.isPlaying) {
        EditorGUILayout.Separator();

        _showPreview = EditorGUILayout.Foldout(_showPreview, "Preview");
        if (_showPreview) {
          DrawPreviewGui();
        }
      }
    }

    private void DrawPreviewGui() {
      Rect previewRect =
          EditorGUILayout.GetControlRect(false, 2 * EditorGUIUtility.singleLineHeight);

      bool prevSustain = _sustain;
      Rect sustainRect = previewRect;
      sustainRect.width = 2 * EditorGUIUtility.fieldWidth;
      _sustain = EditorGUI.ToggleLeft(sustainRect, "Sustain", _sustain);

      // TODO(#161): Add option to snap to semitone grids of an octave.
      Rect pianoRollRect = previewRect;
      pianoRollRect.x += sustainRect.width;
      pianoRollRect.width -= pianoRollRect.x;
      EditorGUI.DrawRect(pianoRollRect, Color.gray);

      Event currentEvent = Event.current;
      Instrument instrument = target as Instrument;

      bool isMouseInPianoRoll = pianoRollRect.Contains(currentEvent.mousePosition);
      if (_sustain || (isMouseInPianoRoll && currentEvent.button == 0 &&
                       (currentEvent.type == EventType.MouseDown ||
                        currentEvent.type == EventType.MouseDrag))) {
        instrument.SetNoteOn(0.0f);
        if (isMouseInPianoRoll) {
          float pitchShift =
              Mathf.Clamp01((currentEvent.mousePosition.x - pianoRollRect.x) / pianoRollRect.width);
          instrument.SetNotePitchShift(0.0f, pitchShift);
        }
      } else if ((prevSustain && !_sustain) ||
                 (currentEvent.button == 0 && currentEvent.type == EventType.MouseUp)) {
        instrument.SetNoteOff(0.0f);
      }
    }

    private bool _showPreview = true;
    private bool _sustain = false;
  }
}
