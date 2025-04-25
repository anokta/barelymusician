using UnityEditor;
using UnityEngine;

namespace Barely {
  [CustomEditor(typeof(Instrument))]
  public class InstrumentEditor : Editor {
    public override void OnInspectorGUI() {
      base.OnInspectorGUI();

      EditorGUILayout.Separator();

      _showPreview = EditorGUILayout.Foldout(_showPreview, "Preview");
      if (_showPreview) {
        DrawPreviewGui();
      } else if (_noteKey != _invalidKey) {
        (target as Instrument).SetNoteOff(NoteKeyToPitch(_noteKey));
        _noteKey = _invalidKey;
      }
    }

    private void DrawPreviewGui() {
      Rect previewRect =
          EditorGUILayout.GetControlRect(false, 2 * EditorGUIUtility.singleLineHeight);

      Rect sustainRect = previewRect;
      sustainRect.width = 2 * EditorGUIUtility.fieldWidth;
      _sustain = EditorGUI.ToggleLeft(sustainRect, "Sustain", _sustain);

      Rect pianoRect = previewRect;
      pianoRect.x += sustainRect.width;
      pianoRect.width -= sustainRect.width;

      float keyWidth = pianoRect.width / (float)_keyCount;

      // Update the preview state.
      Event currentEvent = Event.current;

      bool isMouseInsidePiano = pianoRect.Contains(currentEvent.mousePosition);
      if (isMouseInsidePiano && currentEvent.button == 0 &&
          (currentEvent.type == EventType.MouseDown || currentEvent.type == EventType.MouseDrag)) {
        _isMouseHeld = true;
      } else if (currentEvent.button == 0 && currentEvent.type == EventType.MouseUp) {
        _isMouseHeld = false;
      }

      if (currentEvent.type != EventType.Repaint) {
        return;
      }

      // Play the instrument.
      Instrument instrument = target as Instrument;

      if (_isMouseHeld) {
        int noteKey =
            Mathf.Clamp(Mathf.FloorToInt(_keyCount * (currentEvent.mousePosition.x - pianoRect.x) /
                                         pianoRect.width),
                        0, _semitoneCount);
        if (noteKey != _noteKey) {
          if (_noteKey != _invalidKey) {
            instrument.SetNoteOff(NoteKeyToPitch(_noteKey));
          }
          instrument.SetNoteOn(NoteKeyToPitch(noteKey));
          _noteKey = noteKey;
        }
      } else if (_noteKey != _invalidKey && !_sustain) {
        instrument.SetNoteOff(NoteKeyToPitch(_noteKey));
        _noteKey = _invalidKey;
      }

      // Draw the piano keys.
      for (int i = 0; i < _keyCount; ++i) {
        Rect semitoneRect = pianoRect;
        semitoneRect.width = keyWidth;
        semitoneRect.x = pianoRect.x + (float)i * semitoneRect.width;
        Color keyColor =
            (i == _noteKey) ? Color.green : (_isWhiteKey[i] ? Color.white : Color.black);
        EditorGUI.DrawRect(semitoneRect, keyColor);
      }

      Repaint();
    }

    private float NoteKeyToPitch(int noteKey) {
      return (float)noteKey / _semitoneCount;
    }

    private const int _semitoneCount = 12;
    private const int _keyCount = _semitoneCount + 1;
    private static readonly bool[] _isWhiteKey = new bool[_keyCount] {
      true, false, true, false, true, true, false, true, false, true, false, true, true,
    };
    private const int _invalidKey = -11;

    private bool _isMouseHeld = false;
    private int _noteKey = _invalidKey;
    private bool _showPreview = true;
    private bool _sustain = false;
  }
}
