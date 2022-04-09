using System;
using UnityEngine;

namespace Barely {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    /// Instrument identifier.
    public Int64 Id { get; private set; } = Musician.Api.InvalidId;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Note off callback.
    public delegate void NoteOffCallback(double pitch, double timestamp);
    public event NoteOffCallback OnNoteOff;
    private NoteOffCallback _noteOffCallback = null;

    /// Note on callback.
    public delegate void NoteOnCallback(double pitch, double intensity, double timestamp);
    public event NoteOnCallback OnNoteOn;
    private NoteOnCallback _noteOnCallback = null;

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
      _noteOffCallback = delegate(double pitch, double timestamp) {
        OnNoteOff?.Invoke(pitch, timestamp);
      };
      _noteOnCallback = delegate(double pitch, double intensity, double timestamp) {
        OnNoteOn?.Invoke(pitch, intensity, timestamp);
      };
    }

    protected virtual void OnDestroy() {
      Source = null;
      _noteOffCallback = null;
      _noteOnCallback = null;
    }

    protected virtual void OnEnable() {
      Id = Musician.Api.Instrument_Create(this, ref _noteOffCallback, ref _noteOnCallback);
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      Musician.Api.Instrument_Destroy(this);
      Id = Musician.Api.InvalidId;
    }

    /// Resets all parameters to default value.
    public void ResetAllParameters() {
      Musician.Api.Instrument_ResetAllParameters(this);
    }

    /// Resets parameter to default value.
    ///
    /// @param index Parameter index.
    /// @return True if success, false otherwise.
    public bool ResetParameter(int index) {
      return Musician.Api.Instrument_ResetParameter(this, index);
    }

    /// Sets parameter value.
    ///
    /// @param index Parameter index.
    /// @param value Parameter value.
    /// @return True if success, false otherwise.
    public bool SetParameter(int index, double value) {
      return Musician.Api.Instrument_SetParameter(this, index, value);
    }

    /// Starts playing note.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void StartNote(double pitch, double intensity) {
      Musician.Api.Instrument_StartNote(this, pitch, intensity);
    }

    /// Stops all notes.
    public void StopAllNotes() {
      Musician.Api.Instrument_StopAllNotes(this);
    }

    /// Stops playing note.
    ///
    /// @param pitch Note pitch.
    public void StopNote(double pitch) {
      Musician.Api.Instrument_StopNote(this, pitch);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Api.Instrument_Process(this, data, channels);
    }
  }
}
