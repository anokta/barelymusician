using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Example instrument.
  public class SineInstrument : UnityInstrument {
    // Sine frequency.
    public float frequency = 440.0f;

    // Sine phase.
    private float phase = 0.0f;

    // Current gain.
    private float gain = 0.0f;
    private float targetGain = 0.0f;

    // Sampling interval in Hz.
    private float sampleInterval = 0.0f;

    protected override void Awake() {
      base.Awake();

      phase = 0.0f;
      gain = 0.0f;
      targetGain = 0.0f;
      sampleInterval = 1.0f / (float)(AudioSettings.outputSampleRate);
    }

    public override void UnityProcess(float[] data, int channels) {
      for (int i = 0; i < data.Length; i += channels) {
        gain = Mathf.Lerp(gain, targetGain, 0.01f);
        // Generate next sample.
        float sample = gain * Mathf.Sin(phase * 2.0f * Mathf.PI);
        for (int channel = 0; channel < channels; ++channel) {
          data[i + channel] = sample;
        }
        // Update phase.
        phase += frequency * sampleInterval;
        if (phase >= 1.0f) {
          phase -= 1.0f;
        }
      }
    }

    public override void UnitySetNoteOff(float pitch) {
      targetGain = 0.0f;
    }

    public override void UnitySetNoteOn(float pitch, float intensity) {
      frequency = 440.0f * Mathf.Pow(2.0f, (pitch - 69.0f) / 12.0f);
      phase = 0.0f;
      targetGain = intensity;
    }

    public override void UnitySetParam(int id, float value) { }
  }
}
