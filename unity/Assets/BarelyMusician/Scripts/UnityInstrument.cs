using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument interface.
  public abstract class UnityInstrument : MonoBehaviour, IInstrument {
    // Internal instrument callbacks.
    public BarelyMusician.NoteOffFn noteOffFn = null;
    public BarelyMusician.NoteOnFn noteOnFn = null;
    public BarelyMusician.ProcessFn processFn = null;

    public abstract void NoteOff(float index);
    public abstract void NoteOn(float index, float intensity);
    public abstract void Process(float[] output, int numChannels);


    protected void OnEnable() {
      noteOffFn = NoteOff;
      noteOnFn = NoteOn;
      processFn = delegate (float[] output, int size, int numChannels) { Process(output, numChannels); };
    }
  }
}
