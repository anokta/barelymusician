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
    public OscillatorType OscillatorType = OscillatorType.SINE;

    /// Envelope attack.
    [Min(0.0f)]
    public double Attack = 0.05;

    /// Envelope decay.
    [Min(0.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release.
    [Min(0.0f)]
    public double Release = 0.25;

    /// Number of voices.
    [Range(1, 32)]
    public int NumVoices = 8;

    private void Update() {
      SetParameter(0, (double)OscillatorType);
      SetParameter(1, Attack);
      SetParameter(2, Decay);
      SetParameter(3, Sustain);
      SetParameter(4, Release);
      SetParameter(5, (double)NumVoices);
    }
  }
}
