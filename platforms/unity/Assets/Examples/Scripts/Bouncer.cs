using UnityEngine;

namespace Barely {
  namespace Examples {
    public class Bouncer : MonoBehaviour {
      public Instrument instrument;

      public double rootPitch = 0.0;

      private readonly double[] MajorScale = new double[] { 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0 };

      private int lastIndex = 0;

      private void Update() {
        if (transform.position.y < -10.0f) {
          GameObject.Destroy(gameObject);
        }
      }

      private void OnCollisionEnter(Collision collision) {
        double octaveOffset = lastIndex / MajorScale.Length;
        double pitch =
            rootPitch + octaveOffset + MajorScale[lastIndex++ % MajorScale.Length] / 12.0;
        double intensity = (double)Mathf.Min(1.0f, 0.1f * collision.relativeVelocity.sqrMagnitude);
        instrument.SetNoteOn(pitch, intensity);
        instrument.SetNoteOff(pitch);
      }
    }
  }  // namespace Examples
}  // namespace Barely
