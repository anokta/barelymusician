using UnityEngine;

namespace Barely {
  /// Synthesizer oscillator type.
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

  /// A representation of a simple polyphonic synthesizer instrument with an envelope.
  public class SynthInstrument : Instrument {
    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Oscillator type.
    public OscillatorType OscillatorType = OscillatorType.SINE;

    /// Envelope attack in seconds.
    [Range(0.0f, 60.0f)]
    public double Attack = 0.05;

    /// Envelope decay in seconds.
    [Range(0.0f, 60.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release in seconds.
    [Range(0.0f, 60.0f)]
    public double Release = 0.25;

    /// Number of voices.
    [Range(1, 64)]
    public int VoiceCount = 8;

    private void Update() {
      GetControl(0).Value = Gain;
      GetControl(1).Value = (double)OscillatorType;
      GetControl(2).Value = Attack;
      GetControl(3).Value = Decay;
      GetControl(4).Value = Sustain;
      GetControl(5).Value = Release;
      GetControl(6).Value = (double)VoiceCount;
    }
  }
}  // namespace Barely
