using UnityEngine;

namespace Barely {
  namespace Examples {
    public class PerformerDemo : MonoBehaviour {
      public Instrument instrument;
      public Performer performer;
      public Scale scale;
      public TextMesh text;

      private float _h = 0.0f;
      private float _v = 0.0f;

      public void OnProcess() {
        int degree = Random.Range(-2, 2) * scale.PitchCount + Random.Range(0, scale.PitchCount);
        int pitch = scale.GetPitch(degree);
        instrument.SetNoteOn(pitch);
        performer.ScheduleOneOffTask(delegate() { instrument.SetNoteOff(pitch); },
                                     performer.LoopLength);

        text.text = pitch.ToString();
        _h = Random.Range(0.0f, 1.0f);
        _v = pitch / 80.0f;
      }

      private void Update() {
        text.color = Color.HSVToRGB(_h, 1.0f, _v);
        Camera.main.backgroundColor = Color.HSVToRGB(_h, 1.0f, 1.0f - _v);
      }
    }
  }  // namespace Examples
}  // namespace Barely
