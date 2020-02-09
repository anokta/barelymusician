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

    // Is playing?
    private bool playing = false;

    // Current gain.
    private float gain = 0.0f;

    // Sampling interval in Hz.
    private float sampleInterval = 0.0f;

    protected override void Awake() {
      base.Awake();

      phase = 0.0f;
      playing = false;
      gain = 0.0f;
      sampleInterval = 1.0f / (float)(AudioSettings.outputSampleRate);
    }

    public override void UnityNoteOff(float index) {
      playing = false;
    }

    public override void UnityNoteOn(float index, float intensity) {
      frequency = (index + 1) * 220.0f;
      phase = 0.0f;
      playing = true;
    }

    public override void UnityProcess(float[] data, int channels) {
      for (int i = 0; i < data.Length; i += channels) {
        gain = Mathf.Lerp(gain, playing ? 1.0f : 0.0f, 0.1f);
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
  }
}
