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
      if (_id == BarelyMusician.InvalidId) {
        _id = BarelyMusician.Create(this, AudioSettings.dspTime);
      }
    }

    protected virtual void OnDestroy() {
      _source = null;
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Destroy(_id, AudioSettings.dspTime);
        _id = BarelyMusician.InvalidId;
      }
    }

    protected virtual void OnEnable() {
      _source.Play();
    }

    protected virtual void OnDisable() {
      _source.Stop();
    }

    // Schedules note off.
    public void ScheduleNoteOff(double dspTime, float pitch) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetNoteOff(_id, dspTime, pitch);
      }
    }

    // Schedules note on.
    public void ScheduleNoteOn(double dspTime, float pitch, float intensity) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetNoteOn(_id, dspTime, pitch, intensity);
      }
    }

    // Stops all notes.
    public void SetAllNotesOff() {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetAllNotesOff(_id, AudioSettings.dspTime);
      }
    }

    // Stops playing note with the given |pitch|.
    public void SetNoteOff(float pitch) {
      ScheduleNoteOff(AudioSettings.dspTime, pitch);
    }

    // Starts playing note with the given |pitch| and |intensity|.
    public void SetNoteOn(float pitch, float intensity) {
      ScheduleNoteOn(AudioSettings.dspTime, pitch, intensity);
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
