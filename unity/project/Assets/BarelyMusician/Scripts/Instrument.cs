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
        _id = BarelyMusician.Create(this);
      }
    }

    protected virtual void OnDestroy() {
      _source = null;
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Destroy(_id);
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
    public void ScheduleNoteOff(float pitch, double dspTime) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetNoteOff(_id, pitch, dspTime);
      }
    }

    // Schedules note on.
    public void ScheduleNoteOn(float pitch, float intensity, double dspTime) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetNoteOn(_id, pitch, intensity, dspTime);
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
      ScheduleNoteOff(pitch, AudioSettings.dspTime);
    }

    // Starts playing note with the given |pitch| and |intensity|.
    public void SetNoteOn(float pitch, float intensity) {
      ScheduleNoteOn(pitch, intensity, AudioSettings.dspTime);
    }

    // Sets parameter with the given |id| and |value|.
    public void SetParam(int id, float value) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.SetParam(_id, id, value, AudioSettings.dspTime);
      }
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Process(_id, data, channels, AudioSettings.dspTime);
      }
    }
  }
}
