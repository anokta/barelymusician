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

    // Sampling interval in Hz.
    private float sampleInterval = 0.0f;

    protected void Awake() {
      phase = 0.0f;
      playing = false;
      sampleInterval = 1.0f / (float)(AudioSettings.outputSampleRate);
    }

    public override void NoteOff(float index) {
      playing = false;
    }

    public override void NoteOn(float index, float intensity) {
      //frequency = (index + 1) * 220.0f;
      phase = 0.0f;
      playing = true;
    }

    public override void Process(float[] data, int channels) {
      for (int i = 0; i < data.Length; i += channels) {
        // Generate next sample.
        float sample = playing ? Mathf.Sin(phase * 2.0f * Mathf.PI) : 0.0f;
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
