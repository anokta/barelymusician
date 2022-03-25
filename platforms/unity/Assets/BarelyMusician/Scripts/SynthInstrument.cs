using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// Oscillator type.
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

  /// Simple polyphonic synth instrument.
  public class SynthInstrument : Instrument {
    /// Oscillator type.
    public OscillatorType oscillatorType = OscillatorType.Sine;

    /// Envelope attack.
    public double attack = 0.05;

    /// Envelope decay.
    public double decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double sustain = 1.0;

    /// Envelope release.
    public double release = 0.25;

    /// Number of voices.
    [Range(1, 32)]
    public int numVoices = 8;

    private void Update() {
      SetParameter(0, (double)oscillatorType);
      SetParameter(1, attack);
      SetParameter(2, decay);
      SetParameter(3, sustain);
      SetParameter(4, release);
      SetParameter(5, (double)numVoices);
    }
  }
}
