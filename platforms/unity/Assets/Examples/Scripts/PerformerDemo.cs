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
        int degree = Random.Range(-scale.PitchCount, scale.PitchCount);
        double pitch = scale.GetPitch(degree);
        instrument.SetNoteOn(pitch);
        performer.ScheduleOneOffTask(delegate() { instrument.SetNoteOff(pitch); },
                                     performer.LoopLength);

        text.text = ((int)(12.0 * pitch)).ToString();
        _h = Random.Range(0.0f, 1.0f);
        _v = 0.2f * (float)(pitch + 1.0);
      }

      private void Update() {
        text.color = Color.HSVToRGB(_h, 1.0f, _v);
        Camera.main.backgroundColor = Color.HSVToRGB(_h, 1.0f, 1.0f - _v);
      }
    }
  }  // namespace Examples
}  // namespace Barely
