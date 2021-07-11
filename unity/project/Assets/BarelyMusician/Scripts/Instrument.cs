using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    // Performer id.
    private Int64 _id = BarelyMusician.InvalidId;

    // Audio source.
    private AudioSource _source = null;

    protected virtual void Awake() {
      _source = GetComponent<AudioSource>();
    }

    protected virtual void OnDestroy() {
      _source = null;
    }

    protected virtual void OnEnable() {
      if (_id == BarelyMusician.InvalidId) {
        _id = BarelyMusician.Create(this, AudioSettings.dspTime);
      }
      _source.Play();
    }

    protected virtual void OnDisable() {
      _source.Stop();
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Destroy(_id, AudioSettings.dspTime);
        _id = BarelyMusician.InvalidId;
      }
    }

    // Schedules note off.
    public bool ScheduleNoteOff(double dspTime, float pitch) {
      if (_id != BarelyMusician.InvalidId) {
        return BarelyMusician.SetNoteOff(_id, dspTime, pitch);
      }
      return false;
    }

    // Schedules note on.
    public bool ScheduleNoteOn(double dspTime, float pitch, float intensity) {
      if (_id != BarelyMusician.InvalidId) {
        return BarelyMusician.SetNoteOn(_id, dspTime, pitch, intensity);
      }
      return false;
    }

    // Stops all notes.
    public void SetAllNotesOff() {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetAllNotesOff(_id, AudioSettings.dspTime);
      }
    }

    // Stops playing note with the given |pitch|.
    public bool SetNoteOff(float pitch) {
      return ScheduleNoteOff(AudioSettings.dspTime, pitch);
    }

    // Starts playing note with the given |pitch| and |intensity|.
    public bool SetNoteOn(float pitch, float intensity) {
      return ScheduleNoteOn(AudioSettings.dspTime, pitch, intensity);
    }

    // Sets parameter with the given |id| and |value|.
    public void SetParam(int id, float value) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetParam(_id, AudioSettings.dspTime, id, value);
      }
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Process(_id, AudioSettings.dspTime, data, channels);
      }
    }
  }
}
