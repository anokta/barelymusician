using UnityEditorInternal;
using UnityEngine;

namespace Barely.Examples {
  public class SyncAudioSource : MonoBehaviour {
    public Instrument instrument;
    public Performer performer;
    public AudioSource source;

    private double _pitch = 0.0;

    void Update() {
      if (((Application.platform == RuntimePlatform.Android ||
            Application.platform == RuntimePlatform.IPhonePlayer) &&
           Input.GetMouseButtonDown(0)) ||
          Input.GetKeyDown(KeyCode.Space)) {
        if (!source.isPlaying) {
          source.PlayScheduled(Engine.Timestamp);
          performer.Stop();
          performer.Position = 0.0;
          performer.Play();
        } else {
          performer.Stop();
          source.SetScheduledEndTime(Engine.Timestamp);
        }
      }
      transform.rotation =
          Quaternion.AngleAxis((float)(performer.Position * 90.0), Vector3.forward);
      _pitch = (double)Mathf.PingPong(0.5f * Time.time, 2.0f);
    }

    public void PlayNote(TaskEventType type) {
      if (type == TaskEventType.BEGIN) {
        instrument.SetNoteOn(_pitch);
        instrument.SetNoteOff(_pitch);
      }
    }
  }
}  // namespace Barely.Examples
