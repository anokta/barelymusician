using System;
using UnityEditor;
using UnityEngine;

namespace Barely {
  [CustomEditor(typeof(Sequence))]
  [CanEditMultipleObjects]
  public class SequenceEditor : Editor {
    private SerializedProperty _beginOffset = null;
    private SerializedProperty _beginPosition = null;
    private SerializedProperty _endPosition = null;
    private SerializedProperty _instrument = null;
    private SerializedProperty _isLooping = null;
    private SerializedProperty _loopBeginOffset = null;
    private SerializedProperty _loopLength = null;
    private SerializedProperty _notes = null;

    private Sequence _sequence = null;

    void OnEnable() {
      _beginOffset = serializedObject.FindProperty("_beginOffset");
      _beginPosition = serializedObject.FindProperty("_beginPosition");
      _endPosition = serializedObject.FindProperty("_endPosition");
      _instrument = serializedObject.FindProperty("_instrument");
      _isLooping = serializedObject.FindProperty("_isLooping");
      _loopBeginOffset = serializedObject.FindProperty("_loopBeginOffset");
      _loopLength = serializedObject.FindProperty("_loopLength");
      _notes = serializedObject.FindProperty("_notes");
      _sequence = (Sequence)target;
    }

    public override void OnInspectorGUI() {
      serializedObject.Update();

      // Add clickable script field, as provided by `DrawDefaultInspector`.
      MonoScript script = MonoScript.FromMonoBehaviour(target as MonoBehaviour);
      EditorGUI.BeginDisabledGroup(true);
      EditorGUILayout.ObjectField("Script", script, typeof(MonoScript), false);
      EditorGUI.EndDisabledGroup();

      if (Application.isPlaying) {
        _sequence.BeginOffset =
            EditorGUILayout.DoubleField(_beginOffset.displayName, _beginOffset.doubleValue);
      } else {
        EditorGUILayout.PropertyField(_beginOffset);
      }

      EditorGUILayout.Separator();

      if (Application.isPlaying) {
        _sequence.BeginPosition = Math.Max(
            EditorGUILayout.DoubleField(_beginPosition.displayName, _beginPosition.doubleValue),
            0.0);
        _sequence.EndPosition = Math.Max(
            EditorGUILayout.DoubleField(_endPosition.displayName, _endPosition.doubleValue), 0.0);
      } else {
        EditorGUILayout.PropertyField(_beginPosition);
        EditorGUILayout.PropertyField(_endPosition);
      }

      EditorGUILayout.Separator();

      if (Application.isPlaying) {
        _sequence.Instrument = (Instrument)EditorGUILayout.ObjectField(
            _instrument.displayName, _instrument.objectReferenceValue, typeof(Instrument), true);
      } else {
        EditorGUILayout.PropertyField(_instrument);
      }

      EditorGUILayout.Separator();

      if (Application.isPlaying) {
        _sequence.IsLooping = EditorGUILayout.Toggle(_isLooping.displayName, _isLooping.boolValue);
      } else {
        EditorGUILayout.PropertyField(_isLooping);
      }
      EditorGUI.BeginDisabledGroup(!_isLooping.boolValue);
      ++EditorGUI.indentLevel;
      if (Application.isPlaying) {
        _sequence.LoopBeginOffset =
            EditorGUILayout.DoubleField(_loopBeginOffset.displayName, _loopBeginOffset.doubleValue);
        _sequence.LoopLength = Math.Max(
            EditorGUILayout.DoubleField(_loopLength.displayName, _loopLength.doubleValue),
            0.0);
      } else {
        EditorGUILayout.PropertyField(_loopBeginOffset);
        EditorGUILayout.PropertyField(_loopLength);
      }
      --EditorGUI.indentLevel;
      EditorGUI.EndDisabledGroup();

      EditorGUILayout.Separator();

      if (Application.isPlaying) {
        // TODO(#105): Draw list of notes.
      } else {
        EditorGUILayout.PropertyField(_notes);
      }

      serializedObject.ApplyModifiedProperties();
    }
  }
}
