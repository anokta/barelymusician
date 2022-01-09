using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// Simple polyphonic synth instrument.
  public class SynthInstrument : Instrument {
    public enum OscillatorType {
      /// Sine wave.
      Sine = 0,
      /// Sawtooth wave.
      Saw = 1,
      /// Square wave.
      Square = 2,
      /// White noise.
      Noise = 3,
    }

    /// Envelope attack.
    public float attack = 0.05f;

    /// Envelope decay.
    public float decay = 0.0f;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public float sustain = 1.0f;

    /// Envelope release.
    public float release = 0.25f;

    /// Oscillator type.
    public OscillatorType oscillatorType = OscillatorType.Sine;

    /// Number of voices.
    [Range(1, 32)]
    public int numVoices = 8;

    private void Update() {
      SetParam(0, attack);
      SetParam(1, decay);
      SetParam(2, sustain);
      SetParam(3, release);
      SetParam(4, (float)oscillatorType);
      SetParam(5, (float)numVoices);
    }
  }
}
