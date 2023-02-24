﻿using UnityEngine;

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
    /// Gain.
    [Range(0.0f, 1.0f)]
    public double Gain = 1.0;

    /// Oscillator type.
    public OscillatorType OscillatorType = OscillatorType.SINE;

    /// Envelope attack.
    [Range(0.0f, 60.0f)]
    public double Attack = 0.05;

    /// Envelope decay.
    [Range(0.0f, 60.0f)]
    public double Decay = 0.0;

    /// Envelope sustain.
    [Range(0.0f, 1.0f)]
    public double Sustain = 1.0;

    /// Envelope release.
    [Range(0.0f, 60.0f)]
    public double Release = 0.25;

    /// Number of voices.
    [Range(1, 64)]
    public int VoiceCount = 8;

    private void Update() {
      SetControl(0, Gain);
      SetControl(1, (double)OscillatorType);
      SetControl(2, Attack);
      SetControl(3, Decay);
      SetControl(4, Sustain);
      SetControl(5, Release);
      SetControl(6, (double)VoiceCount);
    }
  }
}