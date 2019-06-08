using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BarelyApi {
  // Instrument interface.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    // Instrument ID.
    public int Id { get; private set; }

    // Internal instrument callbacks.
    private BarelyMusician.ClearCallback clearCallback;
    private BarelyMusician.NoteOffCallback noteOffCallback;
    private BarelyMusician.NoteOnCallback noteOnCallback;
    private BarelyMusician.ProcessCallback processCallback;

    // Clears the instrument.
    public abstract void Clear();

    // Starts playing note with the given |index| and |intensity|.
    public abstract void NoteOn(float index, float intensity);

    // Stops playing note with the given |index|.
    public abstract void NoteOff(float index);

    // Processes the next |output| buffer.
    public abstract void Process(float[] output, int numChannels);

    // Audio source.
    private AudioSource source = null;

    void OnEnable() {
      source = GetComponent<AudioSource>();

      clearCallback = Clear;
      noteOffCallback = NoteOff;
      noteOnCallback = NoteOn;
      processCallback = delegate (float[] output, int size, int numChannels) { Process(output, numChannels); };
      Id = BarelyMusician.Instance.CreateInstrument(clearCallback, noteOffCallback, noteOnCallback, processCallback);
      source.Play();
    }

    void OnDisable() {
      source.Stop();
      BarelyMusician.Instance.DestroyInstrument(this);
      Id = BarelyMusician.InvalidId;

      source = null;
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.ProcessInstrument(this, data);
    }
  }
}
