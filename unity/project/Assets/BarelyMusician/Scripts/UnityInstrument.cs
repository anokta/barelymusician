using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Base Unity instrument.
  public abstract class UnityInstrument : Instrument {
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

    // Note off function.
    public BarelyMusician.UnitySetNoteOffFn SetNoteOffFn {
      get {
        if (_setNoteOffFn == null) {
          _setNoteOffFn = UnitySetNoteOff;
        }
        return _setNoteOffFn;
      }
    }
    private BarelyMusician.UnitySetNoteOffFn _setNoteOffFn = null;

    // Note on function.
    public BarelyMusician.UnitySetNoteOnFn SetNoteOnFn {
      get {
        if (_setNoteOnFn == null) {
          _setNoteOnFn = UnitySetNoteOn;
        }
        return _setNoteOnFn;
      }
    }
    private BarelyMusician.UnitySetNoteOnFn _setNoteOnFn = null;

    // Set parameter function.
    public BarelyMusician.UnitySetParamFn SetParamFn {
      get {
        if (_setParamFn == null) {
          _setParamFn = UnitySetParam;
        }
        return _setParamFn;
      }
    }
    private BarelyMusician.UnitySetParamFn _setParamFn = null;

    // Processes the next |output| buffer.
    public abstract void UnityProcess(float[] output, int numChannels);

    // Stops note with the given |pitch|.
    public abstract void UnitySetNoteOff(float pitch);

    // Starts note with the given |pitch| and |intensity|.
    public abstract void UnitySetNoteOn(float pitch, float intensity);

    // Sets parameter with the given |id| and |value|.
    public abstract void UnitySetParam(int id, float value);
  }
}
