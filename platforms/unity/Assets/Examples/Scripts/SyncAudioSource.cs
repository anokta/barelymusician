using UnityEngine;

namespace Barely {
  namespace Examples {
    public class SyncAudioSource : MonoBehaviour {
      public Instrument instrument;
      public Performer performer;
      public AudioSource source;

      private double notePitch = 0.0;

      void Update() {
        if (((Application.platform == RuntimePlatform.Android ||
              Application.platform == RuntimePlatform.IPhonePlayer) &&
             Input.GetMouseButtonDown(0)) ||
            Input.GetKeyDown(KeyCode.Space)) {
          if (!source.isPlaying) {
            source.PlayScheduled((double)Musician.Timestamp);
            performer.Stop();
            performer.Position = 0.0;
            performer.Play();
          } else {
            performer.Stop();
            source.SetScheduledEndTime((double)Musician.Timestamp);
          }
        }
        transform.rotation =
            Quaternion.AngleAxis((float)performer.Position * 90.0f, Vector3.forward);
        notePitch = (double)(Mathf.PingPong(0.5f * Time.time, 2.0f) - 1.0f);
      }

      public void PlayNote() {
        instrument.SetNoteOn(notePitch);
        instrument.SetNoteOff(notePitch);
      }
    }
  }  // namespace Examples
}  // namespace Barely
