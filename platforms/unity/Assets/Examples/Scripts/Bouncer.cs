using UnityEngine;

namespace Barely {
  namespace Examples {
    public class Bouncer : MonoBehaviour {
      public Instrument instrument;
      public Scale scale;

      private int _lastIndex = 0;
      private double _lastPitch = 0.0;

      private void Update() {
        if (transform.position.y < -100.0f) {
          GameObject.Destroy(gameObject);
        }
      }

      private void OnCollisionEnter(Collision collision) {
        _lastPitch = scale.GetPitch(_lastIndex);
        double intensity = (double)Mathf.Min(1.0f, 0.1f * collision.relativeVelocity.sqrMagnitude);
        instrument.SetNoteOn(_lastPitch, intensity);
      }

      private void OnCollisionExit(Collision collision) {
        instrument.SetNoteOff(_lastPitch);
        ++_lastIndex;
      }
    }
  }  // namespace Examples
}  // namespace Barely
