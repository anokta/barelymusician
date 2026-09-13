using UnityEngine;

namespace Barely.Examples {
  public class Bouncer : MonoBehaviour {
    public Instrument instrument;
    public Scale scale;
    public Sparkler sparkler;

    private int _lastIndex = 0;
    private double _lastPitch = 0.0;

    private void Awake() {
      Color.RGBToHSV(sparkler.noteOnColor, out float h, out float s, out float v);
      h = Random.Range(0.3f, 1.0f);
      sparkler.noteOnColor = Color.HSVToRGB(h, s, v);
    }

    private void Update() {
      if (transform.position.y < -100.0) {
        GameObject.Destroy(gameObject);
      }
    }

    private void OnCollisionEnter(Collision collision) {
      _lastPitch = scale.GetPitch(_lastIndex);
      double gain = System.Math.Min(1.0, 0.1 * collision.relativeVelocity.sqrMagnitude);
      instrument.SetNoteOn(_lastPitch, gain);
    }

    private void OnCollisionExit(Collision collision) {
      instrument.SetNoteOff(_lastPitch);
      ++_lastIndex;
    }
  }
}  // namespace Barely.Examples
