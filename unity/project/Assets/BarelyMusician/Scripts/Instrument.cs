using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    // Instrument id.
    public Int64 Id { get; private set; } = BarelyMusician.InvalidId;

    // Audio source.
    public AudioSource Source { get; private set; } = null;

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
    }

    protected virtual void OnDestroy() {
      Source = null;
    }

    protected virtual void OnEnable() {
      Id = BarelyMusician.Create(this);
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      if (Id != BarelyMusician.InvalidId) {
        BarelyMusician.Destroy(this);
        Id = BarelyMusician.InvalidId;
      }
    }

    // Schedules note off.
    public bool ScheduleNoteOff(double dspTime, float pitch) {
      return BarelyMusician.ScheduleNoteOff(this, dspTime, pitch);
    }

    // Schedules note on.
    public bool ScheduleNoteOn(double dspTime, float pitch, float intensity) {
      return BarelyMusician.ScheduleNoteOn(this, dspTime, pitch, intensity);
    }

    // Stops all notes.
    public bool SetAllNotesOff() {
      return BarelyMusician.SetAllNotesOff(this);
    }

    // Stops all notes.
    public bool SetAllParamsToDefault() {
      return BarelyMusician.SetAllParamsToDefault(this);
    }

    // Stops playing note with the given |pitch|.
    public bool SetNoteOff(float pitch) {
      return BarelyMusician.SetNoteOff(this, pitch);
    }

    // Starts playing note with the given |pitch| and |intensity|.
    public bool SetNoteOn(float pitch, float intensity) {
      return BarelyMusician.SetNoteOn(this, pitch, intensity);
    }

    // Sets parameter with the given |id| and |value|.
    public bool SetParam(int id, float value) {
      return BarelyMusician.SetParam(this, id, value);
    }

    // Sets parameter to default value.
    public bool SetParamToDefault(int id) {
      return BarelyMusician.SetParamToDefault(this, id);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Process(this, data, channels);
    }
  }
}
