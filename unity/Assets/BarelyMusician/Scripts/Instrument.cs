using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument interface.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    // Instrument ID.
    public int Id { get; protected set; } = BarelyMusician.InvalidId;

    // Stops playing note with the given |index|.
    public void NoteOff(float index) {
      BarelyMusician.Instance.NoteOff(this, index);
    }

    // Starts playing note with the given |index| and |intensity|.
    public void NoteOn(float index, float intensity) {
      BarelyMusician.Instance.NoteOn(this, index, intensity);
    }
    public void ScheduleNoteOff(float index, double position) {
      BarelyMusician.Instance.ScheduleNoteOff(this, index, position);
    }

    public void ScheduleNoteOn(float index, float intensity, double position) {
      BarelyMusician.Instance.ScheduleNoteOn(this, index, intensity, position);
    }

    // Stops playing note with the given |index|.
    protected abstract void noteOff(float index);

    // Starts playing note with the given |index| and |intensity|.
    protected abstract void noteOn(float index, float intensity);

    // Processes the next |output| buffer.
    protected abstract void process(float[] output, int numChannels);

    // Internal note off callback.
    protected BarelyMusician.NoteOffCallback noteOffCallback = null;

    // Internal note on callback.
    protected BarelyMusician.NoteOnCallback noteOnCallback = null;

    // Audio source.
    private AudioSource source = null;

    protected virtual void Awake() {
      source = GetComponent<AudioSource>();
    }

    protected virtual void OnDestroy() {
      source = null;
    }

    protected virtual void OnEnable() {
      source.Play();
    }

    protected virtual void OnDisable() {
      source.Stop();
      if (Id != BarelyMusician.InvalidId) {
        BarelyMusician.Instance.Destroy(this);
        Id = BarelyMusician.InvalidId;
      }
    }

    protected virtual void Update() {
      BarelyMusician.Instance.Update();
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.Process(this, data);
    }
  }
}
