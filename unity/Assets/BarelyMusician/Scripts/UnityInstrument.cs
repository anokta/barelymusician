using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument interface.
  [RequireComponent(typeof(AudioSource))]
  public abstract class UnityInstrument : Instrument {
    // Internal instrument callbacks.
    private BarelyMusician.NoteOffFn noteOffFn = null;
    private BarelyMusician.NoteOnFn noteOnFn = null;
    private BarelyMusician.ProcessFn processFn = null;

    protected override void OnEnable() {
      base.OnEnable();
      noteOffFn = NoteOff;
      noteOnFn = NoteOn;
      processFn = delegate (float[] output, int size, int numChannels) { Process(output, numChannels); };
      Id = BarelyMusician.Instance.CreateInstrument(noteOffFn, noteOnFn, processFn);
    }
  }
}
