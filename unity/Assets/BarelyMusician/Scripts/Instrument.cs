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
    private BarelyMusician.ClearFn clearFn;
    private BarelyMusician.NoteOffFn noteOffFn;
    private BarelyMusician.NoteOnFn noteOnFn;
    private BarelyMusician.ProcessFn processFn;

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

      clearFn = Clear;
      noteOffFn = NoteOff;
      noteOnFn = NoteOn;
      processFn = delegate (float[] output, int size, int numChannels) { Process(output, numChannels); };
      Id = BarelyMusician.Instance.CreateInstrument(clearFn, noteOffFn, noteOnFn, processFn);
      source.Play();
    }

    void OnDisable() {
      source.Stop();
      BarelyMusician.Instance.DestroyInstrument(this);
      Id = BarelyMusician.InvalidId;

      source = null;
    }

    void Update() {
      BarelyMusician.Instance.UpdateInstrument();
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.ProcessInstrument(this, data);
    }
  }
}
