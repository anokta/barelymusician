using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public class Instrument : MonoBehaviour {
    /// Instrument id.
    public Int64 Id { get; private set; } = Musician.InvalidId;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Note off event.
    public delegate void NoteOffEvent(double pitch);
    public event NoteOffEvent OnNoteOff;

    /// Note on event.
    public delegate void NoteOnEvent(double pitch, double intensity);
    public event NoteOnEvent OnNoteOn;

    // TODO(#85): Temp shortcut, note callbacks should be private.
    // Note off callback.
    public delegate void NoteOffCallback(double pitch, double timestamp);
    public NoteOffCallback _noteOffCallback = null;

    // Note on callback.
    public delegate void NoteOnCallback(double pitch, double intensity, double timestamp);
    public NoteOnCallback _noteOnCallback = null;

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
    }

    protected virtual void OnDestroy() {
      Source = null;
    }

    protected virtual void OnEnable() {
      if (Id == Musician.InvalidId) {
        _noteOffCallback = delegate(double pitch, double timestamp) {
          OnNoteOff?.Invoke(pitch);
        };
        _noteOnCallback = delegate(double pitch, double intensity, double timestamp) {
          OnNoteOn?.Invoke(pitch, intensity);
        };
        Id = Musician.AddInstrument(this);
      }
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      if (Id != Musician.InvalidId) {
        Musician.RemoveInstrument(this);
        Id = Musician.InvalidId;
      }
    }

    /// Resets all parameters to default value.
    public bool ResetAllParameters() {
      return Musician.ResetAllInstrumentParameters(this);
    }

    /// Resets parameter to default value.
    ///
    /// @param index Parameter index.
    /// @return True if success, false otherwise.
    public bool ResetParameter(int index) {
      return Musician.ResetInstrumentParameter(this, index);
    }

    /// Sets parameter value.
    ///
    /// @param index Parameter index.
    /// @param value Parameter value.
    /// @return True if success, false otherwise.
    public bool SetParameter(int index, double value) {
      return Musician.SetInstrumentParameter(this, index, value);
    }

    /// Starts playing note.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    /// @return True if success, false otherwise.
    public bool StartNote(double pitch, double intensity) {
      return Musician.StartInstrumentNote(this, pitch, intensity);
    }

    /// Stops all notes.
    public bool StopAllNotes() {
      return Musician.StopAllInstrumentNotes(this);
    }

    /// Stops playing note.
    ///
    /// @param pitch Note pitch.
    /// @return True if success, false otherwise.
    public bool StopNote(double pitch) {
      return Musician.StopInstrumentNote(this, pitch);
    }

    private void OnAudioFilterRead(double[] data, int channels) {
      Musician.ProcessInstrument(this, data, channels);
    }
  }
}
