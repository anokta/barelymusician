using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument performer.
  [RequireComponent(typeof(AudioSource))]
  public class Performer : MonoBehaviour {
    // Instrument to perform.
    public IInstrument Instrument {
      get { return _instrument; }
      set {
        if (_instrument != value) {
          _instrument = value;
          BarelyMusician.Instance.SetInstrument(_id, _instrument);
        }
      }
    }
    private IInstrument _instrument;

    // Performer id.
    public int _id = BarelyMusician.InvalidId;

    // Audio source.
    private AudioSource _source;

    private void Awake() {
      _source = GetComponent<AudioSource>();
      if (_id == BarelyMusician.InvalidId) {
        _id = BarelyMusician.Instance.Create();
      }
    }

    private void OnDestroy() {
      _source = null;
      if (_id != BarelyMusician.InvalidId) {
        BarelyMusician.Instance.Destroy(_id);
        _id = BarelyMusician.InvalidId;
      }
    }

    private void OnEnable() {
      _source.Play();
    }

    private void OnDisable() {
      _source.Stop();
    }

    private void Update() {
      BarelyMusician.Instance.UpdateMainThread();
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.UpdateAudioThread();
      BarelyMusician.Instance.Process(_id, data);
    }

    // Stops playing note with the given |index|.
    public void NoteOff(float index) {
      BarelyMusician.Instance.NoteOff(_id, index);
    }

    // Starts playing note with the given |index| and |intensity|.
    public void NoteOn(float index, float intensity) {
      BarelyMusician.Instance.NoteOn(_id, index, intensity);
    }

    public void ScheduleNoteOff(double position, float index) {
      BarelyMusician.Instance.ScheduleNoteOff(_id, position, index);
    }

    public void ScheduleNoteOn(double position, float index, float intensity) {
      BarelyMusician.Instance.ScheduleNoteOn(_id, position, index, intensity);
    }
  }
}
