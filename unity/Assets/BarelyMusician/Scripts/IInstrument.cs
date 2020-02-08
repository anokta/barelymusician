using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Generic instrument interface.
  public interface IInstrument {
    // Stops note with the given |index|.
    void NoteOff(float index);

    // Starts note with the given |index| and |intensity|.
    void NoteOn(float index, float intensity);

    // Processes the next |output| buffer.
    void Process(float[] output, int numChannels);
  }
}