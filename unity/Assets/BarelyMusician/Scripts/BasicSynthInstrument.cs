using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Basic polyphonic synth instrument.
  public class BasicSynthInstrument : Instrument {
    public enum OscillatorType {
      Sine,    // Sine wave.
      Saw,     // Sawtooth wave.
      Square,  // Square wave.
      Noise,   // White noise.
    }

    // Number of instrument voices.
    public int numVoices = 16;

    // Oscillator type.
    public OscillatorType oscillatorType = OscillatorType.Sine;
  }
}
