using System;
using System.Collections;
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

    protected virtual void SetParam(int id, float value) {
      BarelyMusician.SetParam(_id, id, value);
    }

    // Stops all notes.
    public void AllNotesOff() {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.AllNotesOff(_id);
      }
    }

    // Stops playing note with the given |pitch|.
    public void NoteOff(float pitch) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.NoteOff(_id, pitch);
      }
    }

    // Starts playing note with the given |pitch| and |intensity|.
    public void NoteOn(float pitch, float intensity) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.NoteOn(_id, pitch, intensity);
      }
    }

    // Schedules note.
    public void ScheduleNote(double position, double duration, float pitch, float intensity) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.ScheduleNote(_id, position, duration, pitch, intensity);
      }
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Process(_id, data, channels);
      }
    }
  }
}
