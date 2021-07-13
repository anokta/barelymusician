using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Simple polyphonic synth instrument.
  public class SynthInstrument : Instrument {
    public enum OscillatorType {
      Sine,    // Sine wave.
      Saw,     // Sawtooth wave.
      Square,  // Square wave.
      Noise,   // White noise.
    }

    // Gain.
    [Range(0.0f, 1.0f)]
    public float gain = 0.25f;

    // Envelope attack.
    public float attack = 0.05f;

    // Envelope decay.
    public float decay = 0.0f;

    // Envelope sustain.
    [Range(0.0f, 1.0f)]
    public float sustain = 1.0f;

    // Envelope release.
    public float release = 0.25f;

    // Oscillator type.
    public OscillatorType oscillatorType = OscillatorType.Sine;

    // Number of voices.
    [Range(1, 32)]
    public int numVoices = 8;

    private void Update() {
      SetParam(0, gain);
      SetParam(1, attack);
      SetParam(2, decay);
      SetParam(3, sustain);
      SetParam(4, release);
      SetParam(5, (float)oscillatorType);
      SetParam(6, (float)numVoices);
    }
  }
}
