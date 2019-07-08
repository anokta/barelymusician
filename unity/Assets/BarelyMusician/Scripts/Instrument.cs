using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument interface.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    // Note off event.
    public delegate void NoteOffEvent(float index);
    public event NoteOffEvent OnNoteOff;

    // Note on event.
    public delegate void NoteOnEvent(float index, float intensity);
    public event NoteOnEvent OnNoteOn;

    // Instrument ID.
    public int Id { get; protected set; } = BarelyMusician.InvalidId;

    // Clears all notes.
    public void Clear() {
      BarelyMusician.Instance.SetInstrumentAllNotesOff(this);
    }

    // Starts note with the given |index| and |intensity|.
    public void StartNote(float index, float intensity) {
      BarelyMusician.Instance.SetInstrumentNoteOn(this, index, intensity);
    }

    // Stops note with the given |index|.
    public void StopNote(float index) {
      BarelyMusician.Instance.SetInstrumentNoteOff(this, index);
    }

    // Clears all notes.
    protected abstract void AllNotesOff();

    // Stops playing note with the given |index|.
    protected abstract void NoteOff(float index);

    // Starts playing note with the given |index| and |intensity|.
    protected abstract void NoteOn(float index, float intensity);

    // Processes the next |output| buffer.
    protected abstract void Process(float[] output, int numChannels);

    // Internal note off callback.
    protected BarelyMusician.NoteOffCallback noteOffCallback = null;

    // Internal note on callback.
    protected BarelyMusician.NoteOnCallback noteOnCallback = null;

    // Audio source.
    private AudioSource source = null;

    protected virtual void Awake() {
      noteOffCallback = delegate (float index) { OnNoteOff?.Invoke(index); };
      noteOnCallback = delegate (float index, float intensity) { OnNoteOn?.Invoke(index, intensity); };
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
        BarelyMusician.Instance.DestroyInstrument(this);
        Id = BarelyMusician.InvalidId;
      }
    }

    protected virtual void Update() {
      BarelyMusician.Instance.UpdateInstrument();
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.ProcessInstrument(this, data);
    }
  }
}
