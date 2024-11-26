using UnityEngine;

namespace Barely {
  namespace Examples {
    public class PerformerDemo : MonoBehaviour {
      public Instrument instrument;
      public Performer performer;
      public Scale scale;
      public TextMesh text;

      private int _beat = 0;

      private float _h = 0.0f;
      private float _v = 0.0f;

      public void OnProcess() {
        if (_beat % 4 == 0) {
          instrument.FilterType = (_beat % 8 == 0) ? FilterType.LOW_PASS : FilterType.HIGH_PASS;
        }
        _beat = (_beat + 1) % 8;

        int degree = Random.Range(-scale.PitchCount, scale.PitchCount);
        float pitch = scale.GetPitch(degree);
        instrument.SetNoteOn(pitch);
        performer.ScheduleOneOffTask(delegate() { instrument.SetNoteOff(pitch); },
                                     performer.LoopLength);

        text.text = ((int)(12.0f * pitch)).ToString();
        _h = Random.Range(0.0f, 1.0f);
        _v = 0.2f * (pitch + 1.0f);
      }

      private void Update() {
        text.color = Color.HSVToRGB(_h, 1.0f, _v);
        Camera.main.backgroundColor = Color.HSVToRGB(_h, 1.0f, 1.0f - _v);
      }
    }
  }  // namespace Examples
}  // namespace Barely
