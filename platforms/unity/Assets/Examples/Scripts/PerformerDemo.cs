using UnityEngine;

namespace Barely {
  namespace Examples {
    public class PerformerDemo : MonoBehaviour {
      public Instrument instrument;
      public Performer performer;
      public TextMesh text;

      public int[] scale = new int[6] { 0, 2, 4, 7, 9, 11 };

      private float _h = 0.0f;
      private float _v = 0.0f;

      public void OnProcess() {
        int midiNumber = 33 + Random.Range(0, 4) * 12 + scale[Random.Range(0, scale.Length)];
        double pitch = Musician.GetPitchFromMidiKey(midiNumber);
        instrument.SetNoteOn(pitch);
        performer.ScheduleOneOffTask(delegate() { instrument.SetNoteOff(pitch); },
                                     performer.LoopLength);

        text.text = midiNumber.ToString();
        _h = Random.Range(0.0f, 1.0f);
        _v = midiNumber / 80.0f;
      }

      private void Update() {
        text.color = Color.HSVToRGB(_h, 1.0f, _v);
        Camera.main.backgroundColor = Color.HSVToRGB(_h, 1.0f, 1.0f - _v);
      }
    }
  }  // namespace Examples
}  // namespace Barely
