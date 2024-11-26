using UnityEngine;

namespace Barely {
  namespace Examples {
    // A simple theremin that is contolled by the mouse position on the screen.
    public class Theremin : MonoBehaviour {
      public Instrument instrument = null;
      public Texture2D texture = null;
      public Color color = Color.white;

      public void OnGUI() {
        color.a =
            Mathf.Lerp(color.a, instrument.IsNoteOn(0.0f) ? 1.0f : 0.0f, 8.0f * Time.deltaTime);
        GUI.color = color;
        float size = 0.05f * Mathf.Min(Screen.width, Screen.height);
        GUI.DrawTexture(new Rect(Input.mousePosition.x - 0.5f * size,
                                 Screen.height - Input.mousePosition.y - 0.5f * size, size, size),
                        texture);
      }

      public void Update() {
        instrument.PitchShift =
            2.0f * Mathf.Clamp(Input.mousePosition.x / Screen.width, 0.0f, 1.0f) - 1.0f;
        instrument.Gain = 20.0f * Mathf.Log10(Input.mousePosition.y / Screen.height);
        if (Input.GetMouseButtonDown(0)) {
          instrument.SetNoteOn(0.0f, 1.0f);
        } else if (Input.GetMouseButtonUp(0)) {
          instrument.SetNoteOff(0.0f);
        }
      }
    }
  }  // namespace Examples
}  // namespace Barely
