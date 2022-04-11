using UnityEngine;

namespace Barely {
  /// Oscillator type.
  public enum OscillatorType {
    /// Sine wave.
    [InspectorName("Sine")] SINE = 0,
    /// Sawtooth wave.
    [InspectorName("Saw")] SAW = 1,
    /// Square wave.
    [InspectorName("Square")] SQUARE = 2,
    /// White noise.
    [InspectorName("Noise")] NOISE = 3,
  }

  /// Simple polyphonic synth instrument.
  public class SynthInstrument : Instrument {
    /// Oscillator type.
    public OscillatorType oscillatorType = OscillatorType.SINE;

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
