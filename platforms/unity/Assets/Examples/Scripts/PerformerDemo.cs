using UnityEngine;

namespace Barely {
  namespace Examples {
    public class PerformerDemo : MonoBehaviour {
      public Instrument instrument;
      public Performer performer;
      public TextMesh text;

      public int[] scale = new int[6] { 0, 2, 4, 7, 9, 11 };

      public void OnProcess() {
        int midiNumber = 33 + Random.Range(0, 4) * 12 + scale[Random.Range(0, scale.Length)];
        double pitch = Musician.GetPitchFromMidiKey(midiNumber);
        instrument.SetNoteOn(pitch);
        performer.ScheduleOneOffTask(delegate() { instrument.SetNoteOff(pitch); },
                                     performer.LoopLength);

        float h = Random.Range(0.0f, 1.0f);
        float s = Random.Range(0.25f, 0.75f);
        float v = midiNumber / 80.0f;
        text.color = Color.HSVToRGB(h, s, v);
        text.text = midiNumber.ToString();
        Camera.main.backgroundColor = Color.HSVToRGB(h, s, 1.0f - v);
      }
    }
  }  // namespace Examples
}  // namespace Barely
