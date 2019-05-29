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
    private BarelyMusician.NoteOffCallback noteOffCallback;
    private BarelyMusician.NoteOnCallback noteOnCallback;
    private BarelyMusician.ProcessCallback processCallback;
    private BarelyMusician.ResetCallback resetCallback;

    // Starts playing note with the given |index| and |intensity|.
    public abstract void NoteOn(float index, float intensity);

    // Stops playing note with the given |index|.
    public abstract void NoteOff(float index);

    // Processes the next |output| buffer.
    public abstract void Process(float[] output, int numChannels);

    // Resets the instrument.
    public abstract void Reset();

    void OnEnable() {
      noteOffCallback = NoteOff;
      noteOnCallback = NoteOn;
      processCallback = delegate (float[] output, int size, int numChannels) {
        Process(output, numChannels);
      };
      resetCallback = Reset;
      Id = BarelyMusician.Instance.CreateInstrument(noteOffCallback, noteOnCallback,
                                                    processCallback, resetCallback);
    }

    void OnDisable() {
      BarelyMusician.Instance.DestroyInstrument(this);
      Id = BarelyMusician.InvalidId;
    }

    void OnAudioFilterRead(float[] data, int channels) {
      BarelyMusician.Instance.ProcessInstrument(this, data);
    }
  }
}
