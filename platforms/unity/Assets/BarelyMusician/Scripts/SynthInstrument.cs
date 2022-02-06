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
      SetParameter(0, attack);
      SetParameter(1, decay);
      SetParameter(2, sustain);
      SetParameter(3, release);
      SetParameter(4, (float)oscillatorType);
      SetParameter(5, (float)numVoices);
    }
  }
}
