using UnityEngine;

namespace Barely.Examples {
  public class PerformerDemo : MonoBehaviour {
    public Instrument instrument;
    public Performer performer;
    public Scale scale;
    public TextMesh text;

    private int _beat = 0;

    private float _h = 0.0f;
    private float _v = 0.0f;

    private float _pitch = 0.0f;

    public void OnProcess(TaskState state) {
      if (state == TaskState.BEGIN) {
        if (_beat % 4 == 0) {
          instrument.FilterType = (_beat % 8 == 0) ? FilterType.LOW_PASS : FilterType.HIGH_PASS;
        }
        _beat = (_beat + 1) % 8;

        int degree = Random.Range(-scale.PitchCount, scale.PitchCount);
        _pitch = scale.GetPitch(degree);
        instrument.SetNoteOn(_pitch);

        text.text = ((int)(12.0f * _pitch)).ToString();
        _h = Random.Range(0.0f, 1.0f);
        _v = 0.2f * (_pitch + 1.0f);
      } else if (state == TaskState.END) {
        instrument.SetNoteOff(_pitch);
      }
    }

    private void Update() {
      text.color = Color.HSVToRGB(_h, 1.0f, _v);
      Camera.main.backgroundColor = Color.HSVToRGB(_h, 1.0f, 1.0f - _v);
    }
  }
}  // namespace Barely.Examples
