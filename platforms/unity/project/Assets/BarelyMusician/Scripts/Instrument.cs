using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    /// Instrument id.
    public Int64 Id { get; private set; } = BarelyMusician.InvalidId;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
    }

    protected virtual void OnDestroy() {
      Source = null;
    }

    protected virtual void OnEnable() {
      if (Id == BarelyMusician.InvalidId) {
        Id = BarelyMusician.AddInstrument(this);
      }
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      if (Id != BarelyMusician.InvalidId) {
        BarelyMusician.RemoveInstrument(this);
        Id = BarelyMusician.InvalidId;
      }
    }

    /// Stops all notes.
    public bool SetAllNotesOff() {
      return BarelyMusician.SetAllInstrumentNotesOff(this);
    }

    /// Sets all parameters to default.
    public bool SetAllParamsToDefault() {
      return BarelyMusician.SetAllInstrumentParamsToDefault(this);
    }

    /// Stops playing note.
    ///
    /// @param pitch Note pitch.
    /// @return True if success, false otherwise.
    public bool SetNoteOff(float pitch) {
      return BarelyMusician.SetInstrumentNoteOff(this, pitch);
    }

    /// Starts playing note.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    /// @return True if success, false otherwise.
    public bool SetNoteOn(float pitch, float intensity) {
      return BarelyMusician.SetInstrumentNoteOn(this, pitch, intensity);
    }

    /// Sets parameter value.
    ///
    /// @param id Parameter id.
    /// @param value Parameter value.
    /// @return True if success, false otherwise.
    public bool SetParam(int id, float value) {
      return BarelyMusician.SetInstrumentParam(this, id, value);
    }

    /// Sets parameter to default value.
    ///
    /// @param id Parameter id.
    /// @return True if success, false otherwise.
    public bool SetParamToDefault(int id) {
      return BarelyMusician.SetInstrumentParamToDefault(this, id);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.ProcessInstrument(this, data, channels);
    }
  }
}
