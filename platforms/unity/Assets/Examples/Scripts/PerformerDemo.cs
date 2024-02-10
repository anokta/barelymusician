using UnityEngine;

namespace Barely {
  namespace Examples {
    public class PerformerDemo : MonoBehaviour {
      public Instrument instrument;
      public HighPassEffect effect;
      public Performer performer;
      public TextMesh text;

      public double cutoffLfoAmplitude = 8000;

      public int[] scale = new int[6] { 0, 2, 4, 7, 9, 11 };

      private int _beat = 0;
      private int _period = 8;
      private bool _isIncreasing = true;

      private float _h = 0.0f;
      private float _s = 0.0f;
      private float _v = 0.0f;

      public void OnProcess() {
        if (_beat++ % _period == 0) {
          _beat %= _period;
          if (_isIncreasing) {
            _period = 4 * Random.Range(2, 6);
          }
          effect.SetCutoffFrequency(effect.CutoffFrequency, ComputeSlopePerBeat());
          _isIncreasing = !_isIncreasing;
        }

        int midiNumber = 33 + Random.Range(0, 4) * 12 + scale[Random.Range(0, scale.Length)];
        double pitch = Musician.GetPitchFromMidiKey(midiNumber);
        instrument.SetNoteOn(pitch);
        performer.ScheduleOneOffTask(delegate() { instrument.SetNoteOff(pitch); },
                                     performer.LoopLength);

        text.text = midiNumber.ToString();
        _h = Random.Range(0.0f, 1.0f);
        _v = midiNumber / 80.0f;
      }

      private void OnEnable() {
        Musician.OnTempo += OnTempo;
      }

      private void OnDisable() {
        Musician.OnTempo -= OnTempo;
      }

      private void Update() {
        _s = (float)(0.5 - effect.CutoffFrequency / cutoffLfoAmplitude);
        text.color = Color.HSVToRGB(_h, _s, _v);
        Camera.main.backgroundColor = Color.HSVToRGB(_h, _s, 1.0f - _v);
      }

      private double ComputeSlopePerBeat() {
        return Musician.GetBeatsFromSeconds(
            (_isIncreasing ? cutoffLfoAmplitude : -cutoffLfoAmplitude) /
            (performer.LoopLength * _period));
      }

      private void OnTempo(double tempo) {
        if (performer.IsPlaying) {
          effect.SetCutoffFrequency(effect.CutoffFrequency, ComputeSlopePerBeat());
        }
      }
    }
  }  // namespace Examples
}  // namespace Barely
