using UnityEditorInternal;
using UnityEngine;

namespace Barely.Examples {
  public class SyncAudioSource : MonoBehaviour {
    public Instrument instrument;
    public Performer performer;
    public AudioSource source;

    private Lfo _lfo = new Lfo();

    void Awake() {
      Engine.Speed = 2.0;
      _lfo.Speed = 0.125;
    }

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
          _lfo.Phase = 0.0;
        } else {
          performer.Stop();
          source.SetScheduledEndTime(Engine.Timestamp);
        }
      }
      transform.rotation = Quaternion.AngleAxis((float)performer.Position * 90.0f, Vector3.forward);
    }

    public void PlayNote(TaskEventType type) {
      if (type == TaskEventType.BEGIN) {
        float pitch = _lfo.Evaluate();
        instrument.SetNoteOn(pitch);
        instrument.SetNoteOff(pitch);
      }
    }
  }
}  // namespace Barely.Examples
