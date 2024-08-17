using UnityEngine;

namespace Barely {
  namespace Examples {
    public class SyncAudioSource : MonoBehaviour {
      public Instrument instrument;
      public Performer performer;
      public AudioSource source;

      private double _note = 440.0;

      void Update() {
        if (((Application.platform == RuntimePlatform.Android ||
              Application.platform == RuntimePlatform.IPhonePlayer) &&
             Input.GetMouseButtonDown(0)) ||
            Input.GetKeyDown(KeyCode.Space)) {
          if (!source.isPlaying) {
            source.PlayScheduled(Musician.Timestamp);
            performer.Stop();
            performer.Position = 0.0;
            performer.Play();
          } else {
            performer.Stop();
            source.SetScheduledEndTime(Musician.Timestamp);
          }
        }
        transform.rotation =
            Quaternion.AngleAxis((float)performer.Position * 90.0f, Vector3.forward);
        _note = (double)(440.0f * Mathf.PingPong(0.5f * Time.time, 2.0f));
      }

      public void PlayNote() {
        instrument.SetNoteOn(_note);
        instrument.SetNoteOff(_note);
      }
    }
  }  // namespace Examples
}  // namespace Barely
