using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Base Unity instrument.
  public abstract class UnityInstrument : Instrument {
    // Note off function.
    public BarelyMusician.UnityNoteOffFn NoteOffFn {
      get {
        if (_noteOffFn == null) {
          _noteOffFn = UnityNoteOff;
        }
        return _noteOffFn;
      }
    }
    private BarelyMusician.UnityNoteOffFn _noteOffFn = null;
    
    // Note on function.
    public BarelyMusician.UnityNoteOnFn NoteOnFn {
      get {
        if (_noteOnFn == null) {
          _noteOnFn = UnityNoteOn;
        }
        return _noteOnFn;
      }
    }
    private BarelyMusician.UnityNoteOnFn _noteOnFn = null;
    
    // Process function.
    public BarelyMusician.UnityProcessFn ProcessFn {
      get {
        if (_processFn == null) {
          _processFn = delegate (float[] output, int size, int numChannels) {
            UnityProcess(output, numChannels);
          };
        }
        return _processFn;
      }
    }
    private BarelyMusician.UnityProcessFn _processFn = null;

    // Stops note with the given |index|.
    public abstract void UnityNoteOff(float index);

    // Starts note with the given |index| and |intensity|.
    public abstract void UnityNoteOn(float index, float intensity);

    // Processes the next |output| buffer.
    public abstract void UnityProcess(float[] output, int numChannels);
  }
}
