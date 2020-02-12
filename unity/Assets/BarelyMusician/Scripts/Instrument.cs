﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    // Performer id.
    private int _id = BarelyMusician.InvalidId;

    // Audio source.
    private AudioSource _source = null;

    protected virtual void Awake() {
      _source = GetComponent<AudioSource>();
      if (_id == BarelyMusician.InvalidId) {
        _id = BarelyMusician.Instance.Create(this);
      }
    }

    protected virtual void OnDestroy() {
      _source = null;
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Instance.Destroy(_id);
        _id = BarelyMusician.InvalidId;
      }
    }

    protected virtual void OnEnable() {
      _source.Play();
    }

    protected virtual void OnDisable() {
      _source.Stop();
    }

    // Stops playing note with the given |index|.
    public void NoteOff(float index) {
      BarelyMusician.Instance.NoteOff(_id, index);
    }

    // Starts playing note with the given |index| and |intensity|.
    public void NoteOn(float index, float intensity) {
      BarelyMusician.Instance.NoteOn(_id, index, intensity);
    }

    // Schedules note off.
    public void ScheduleNoteOff(double position, float index) {
      BarelyMusician.Instance.ScheduleNoteOff(_id, position, index);
    }

    // Schedules note on.
    public void ScheduleNoteOn(double position, float index, float intensity) {
      BarelyMusician.Instance.ScheduleNoteOn(_id, position, index, intensity);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.Process(_id, data, channels);
    }
  }
}
