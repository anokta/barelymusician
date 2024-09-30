using UnityEngine;

namespace Barely {
  namespace Examples {
    public class Bouncer : MonoBehaviour {
      public Instrument instrument;
      public Scale scale;

      private int _lastIndex = 0;

      private void Update() {
        if (transform.position.y < -10.0f) {
          GameObject.Destroy(gameObject);
        }
      }

      private void OnCollisionEnter(Collision collision) {
        int pitch = scale.GetPitch(_lastIndex++);
        double intensity = (double)Mathf.Min(1.0f, 0.1f * collision.relativeVelocity.sqrMagnitude);
        instrument.SetNoteOn(pitch, intensity);
        instrument.SetNoteOff(pitch);
      }
    }
  }  // namespace Examples
}  // namespace Barely
