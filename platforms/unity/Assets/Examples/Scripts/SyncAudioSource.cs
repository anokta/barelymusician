using UnityEngine;
using Barely;

public class SyncAudioSource : MonoBehaviour {
  public Instrument instrument;
  public Performer performer;
  public AudioSource source;

  [Range(-2.0f, 2.0f)]
  public double notePitch = 0.0;

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
    transform.rotation = Quaternion.AngleAxis((float)performer.Position * 90.0f, Vector3.forward);
  }

  public void PlayNote() {
    instrument.SetNoteOn(notePitch);
    instrument.SetNoteOff(notePitch);
  }
}
