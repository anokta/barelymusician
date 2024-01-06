using UnityEngine;
using UnityEditor;

namespace Barely {
  [CustomPropertyDrawer(typeof(Rational))]
  public class RationalDrawer : PropertyDrawer {
    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label) {
      EditorGUI.BeginProperty(position, label, property);

      position = EditorGUI.PrefixLabel(position, GUIUtility.GetControlID(FocusType.Passive), label);

      int indent = EditorGUI.indentLevel;
      EditorGUI.indentLevel = 0;

      float propertyWidth = position.width / 3;

      float labelWidth = EditorGUIUtility.labelWidth;
      EditorGUIUtility.labelWidth = propertyWidth / 2;

      Rect numeratorRect = new Rect(position.x, position.y, propertyWidth, position.height);
      EditorGUI.PropertyField(numeratorRect, property.FindPropertyRelative("Numerator"),
                              new GUIContent("Numerator"));

      Rect separatorRect =
          new Rect(position.x + propertyWidth, position.y, propertyWidth, position.height);
      var centeredStyle = GUI.skin.GetStyle("Label");
      centeredStyle.alignment = TextAnchor.UpperCenter;
      EditorGUI.LabelField(separatorRect, "/", centeredStyle);

      Rect denominatorRect =
          new Rect(position.x + 2 * propertyWidth, position.y, propertyWidth, position.height);
      EditorGUI.PropertyField(denominatorRect, property.FindPropertyRelative("Denominator"),
                              new GUIContent("Denominator"));

      // Set indent back to what it was
      EditorGUI.indentLevel = indent;
      EditorGUIUtility.labelWidth = labelWidth;

      EditorGUI.EndProperty();
    }
  }
}  // namespace Barely
