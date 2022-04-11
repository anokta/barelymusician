using System;
using UnityEngine;

namespace Barely {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Native.InvalidId;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Note off callback.
    ///
    /// @param pitch Note pitch.
    /// @param dspTime Note off time in seconds.
    public delegate void NoteOffCallback(double pitch, double dspTime);
    public event NoteOffCallback OnNoteOff;
    private NoteOffCallback _noteOffCallback = null;

    /// Note on callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    /// @param dspTime Note on time in seconds.
    public delegate void NoteOnCallback(double pitch, double intensity, double dspTime);
    public event NoteOnCallback OnNoteOn;
    private NoteOnCallback _noteOnCallback = null;

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
      _noteOffCallback = delegate(double pitch, double dspTime) {
        OnNoteOff?.Invoke(pitch, dspTime);
      };
      _noteOnCallback = delegate(double pitch, double intensity, double dspTime) {
        OnNoteOn?.Invoke(pitch, intensity, dspTime);
      };
    }

    protected virtual void OnDestroy() {
      Source = null;
      _noteOffCallback = null;
      _noteOnCallback = null;
    }

    protected virtual void OnEnable() {
      Id = Musician.Native.Instrument_Create(this, ref _noteOffCallback, ref _noteOnCallback);
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      Musician.Native.Instrument_Destroy(this);
      Id = Musician.Native.InvalidId;
    }

    /// Returns parameter value.
    ///
    /// @param index Parameter index.
    /// @return Parameter value.
    public double GetParameter(int index) {
      return Musician.Native.Instrument_GetParameter(this, index);
    }

    /// Returns whether note is playing or not.
    ///
    /// @param Note pitch.
    /// @return True if playing, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Native.Instrument_IsNoteOn(this, pitch);
    }

    /// Resets all parameters to default value.
    public void ResetAllParameters() {
      Musician.Native.Instrument_ResetAllParameters(this);
    }

    /// Resets parameter to default value.
    ///
    /// @param index Parameter index.
    public void ResetParameter(int index) {
      Musician.Native.Instrument_ResetParameter(this, index);
    }

    /// Sets parameter value.
    ///
    /// @param index Parameter index.
    /// @param value Parameter value.
    public void SetParameter(int index, double value) {
      Musician.Native.Instrument_SetParameter(this, index, value);
    }

    /// Starts playing note.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void StartNote(double pitch, double intensity) {
      Musician.Native.Instrument_StartNote(this, pitch, intensity);
    }

    /// Stops all notes.
    public void StopAllNotes() {
      Musician.Native.Instrument_StopAllNotes(this);
    }

    /// Stops playing note.
    ///
    /// @param pitch Note pitch.
    public void StopNote(double pitch) {
      Musician.Native.Instrument_StopNote(this, pitch);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Native.Instrument_Process(this, data, channels);
    }
  }
}
