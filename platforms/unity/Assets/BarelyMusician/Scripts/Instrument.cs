using System;
using UnityEngine;

namespace Barely {
  // Instrument.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Native.InvalidId;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Note off callback.
    ///
    /// @param pitch Note pitch.
    /// @param dspTime Note off time in seconds.
    public delegate void NoteOffEventCallback(double pitch, double dspTime);
    public event NoteOffEventCallback OnNoteOff;
    private NoteOffEventCallback _noteOffCallback = null;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<double> {}
    public NoteOffEvent NoteOff;

    /// Note on callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    /// @param dspTime Note on time in seconds.
    public delegate void NoteOnEventCallback(double pitch, double intensity, double dspTime);
    public event NoteOnEventCallback OnNoteOn;
    private NoteOnEventCallback _noteOnCallback = null;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<double, double> {}
    public NoteOnEvent NoteOn;

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
      _noteOffCallback = delegate(double pitch, double dspTime) {
        OnNoteOff?.Invoke(pitch, dspTime);
        NoteOff?.Invoke((float)pitch);
      };
      _noteOnCallback = delegate(double pitch, double intensity, double dspTime) {
        OnNoteOn?.Invoke(pitch, intensity, dspTime);
        NoteOn?.Invoke((float)pitch, (float)intensity);
      };
    }

    protected virtual void OnDestroy() {
      Source = null;
      _noteOffCallback = null;
      _noteOnCallback = null;
    }

    protected virtual void OnEnable() {
      Id = Musician.Native.Instrument_Create(this, _noteOffCallback, _noteOnCallback);
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      Musician.Native.Instrument_Destroy(this);
      Id = Musician.Native.InvalidId;
    }

    /// Returns control value.
    ///
    /// @param index Control index.
    /// @return Control value.
    public double GetControl(int index) {
      return Musician.Native.Instrument_GetControl(this, index);
    }

    /// Returns whether note is playing or not.
    ///
    /// @param Note pitch.
    /// @return True if playing, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Native.Instrument_IsNoteOn(this, pitch);
    }

    /// Resets all controls to default value.
    public void ResetAllControls() {
      Musician.Native.Instrument_ResetAllControls(this);
    }

    /// Resets control to default value.
    ///
    /// @param index Control index.
    public void ResetControl(int index) {
      Musician.Native.Instrument_ResetControl(this, index);
    }

    /// Sets control value.
    ///
    /// @param index Control index.
    /// @param value Control value.
    /// @param value Control slope in value change per second.
    public void SetControl(int index, double value, double slope = 0.0) {
      Musician.Native.Instrument_SetControl(this, index, value, slope);
    }

    /// Sets data.
    ///
    /// @param data Data.
    public void SetData(byte[] data) {
      Musician.Native.Instrument_SetData(this, data);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Native.Instrument_SetAllNotesOff(this);
    }

    /// Sets note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Native.Instrument_SetNoteOff(this, pitch);
    }

    /// Sets note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch, double intensity = 1.0) {
      Musician.Native.Instrument_SetNoteOn(this, pitch, intensity);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Native.Instrument_Process(this, data, channels);
    }
  }
}
