using System;
using UnityEngine;

namespace Barely {
  /// Instrument.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Control event callback.
    ///
    /// @param index Control index.
    /// @param control Control value.
    public delegate void ControlEventCallback(int index, double value);
    public event ControlEventCallback OnControl;
    private ControlEventCallback _controlEventCallback = null;

    [Serializable]
    public class ControlEvent : UnityEngine.Events.UnityEvent<int, float> {}
    public ControlEvent OnControlEvent;

    /// Note control event callback.
    ///
    /// @param pitch Note pitch.
    /// @param index Note control index.
    /// @param control Note control value.
    public delegate void NoteControlEventCallback(double pitch, int index, double value);
    public event NoteControlEventCallback OnNoteControl;
    private NoteControlEventCallback _noteControlEventCallback = null;

    [Serializable]
    public class NoteControlEvent : UnityEngine.Events.UnityEvent<float, int, float> {}
    public NoteControlEvent OnNoteControlEvent;

    /// Note off event callback.
    ///
    /// @param pitch Note pitch.
    public delegate void NoteOffEventCallback(double pitch);
    public event NoteOffEventCallback OnNoteOff;
    private NoteOffEventCallback _noteOffEventCallback = null;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on event callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public delegate void NoteOnEventCallback(double pitch, double intensity);
    public event NoteOnEventCallback OnNoteOn;
    private NoteOnEventCallback _noteOnEventCallback = null;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<float, float> {}
    public NoteOnEvent OnNoteOnEvent;

    /// Returns a control value.
    ///
    /// @param index Control index.
    /// @return Control value.
    public double GetControl(int index) {
      return Musician.Internal.Instrument_GetControl(_id, index);
    }

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param index Control index.
    /// @return Control value.
    public double GetNoteControl(double pitch, int index) {
      return Musician.Internal.Instrument_GetNoteControl(_id, pitch, index);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param Note pitch.
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Instrument_IsNoteOn(_id, pitch);
    }

    /// Resets all control values.
    public void ResetAllControls() {
      Musician.Internal.Instrument_ResetAllControls(_id);
    }

    /// Resets all note control values.
    ///
    /// @param pitch Note pitch.
    public void ResetAllNoteControls(double pitch) {
      Musician.Internal.Instrument_ResetAllNoteControls(_id, pitch);
    }

    /// Resets a control value.
    ///
    /// @param index Control index.
    public void ResetControl(int index) {
      Musician.Internal.Instrument_ResetControl(_id, index);
    }

    /// Resets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param index Note control index.
    public void ResetNoteControl(double pitch, int index) {
      Musician.Internal.Instrument_ResetNoteControl(_id, pitch, index);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_id);
    }

    /// Sets a control value.
    ///
    /// @param index Control index.
    /// @param value Control value.
    /// @param slopePerBeat Control slope in value change per beat.
    public void SetControl(int index, double value, double slopePerBeat = 0.0) {
      Musician.Internal.Instrument_SetControl(_id, index, value, slopePerBeat);
    }

    /// Sets data.
    ///
    /// @param data Data.
    public void SetData(byte[] data) {
      Musician.Internal.Instrument_SetData(_id, data);
    }

    /// Sets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param index Note control index.
    /// @param value Note control value.
    /// @param slopePerBeat Note control slope in value change per beat.
    public void SetNoteControl(double pitch, int index, double value, double slopePerBeat = 0.0) {
      Musician.Internal.Instrument_SetNoteControl(_id, pitch, index, value, slopePerBeat);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Internal.Instrument_SetNoteOff(_id, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch, double intensity = 1.0) {
      Musician.Internal.Instrument_SetNoteOn(_id, pitch, intensity);
    }

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
      _controlEventCallback = delegate(int index, double value) {
        OnControl?.Invoke(index, value);
        OnControlEvent?.Invoke(index, (float)value);
      };
      _noteControlEventCallback = delegate(double pitch, int index, double value) {
        OnNoteControl?.Invoke(pitch, index, value);
        OnNoteControlEvent?.Invoke((float)pitch, index, (float)value);
      };
      _noteOffEventCallback = delegate(double pitch) {
        OnNoteOff?.Invoke(pitch);
        OnNoteOffEvent?.Invoke((float)pitch);
      };
      _noteOnEventCallback = delegate(double pitch, double intensity) {
        OnNoteOn?.Invoke(pitch, intensity);
        OnNoteOnEvent?.Invoke((float)pitch, (float)intensity);
      };
    }

    protected virtual void OnDestroy() {
      Source = null;
      _controlEventCallback = null;
      _noteControlEventCallback = null;
      _noteOffEventCallback = null;
      _noteOnEventCallback = null;
    }

    protected virtual void OnEnable() {
      _id = Musician.Internal.Instrument_Create(this, _controlEventCallback,
                                                _noteControlEventCallback, _noteOffEventCallback,
                                                _noteOnEventCallback);
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      Musician.Internal.Instrument_Destroy(_id);
      _id = Musician.Internal.InvalidId;
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_id, data, channels);
    }

    // Identifier.
    private Int64 _id = Musician.Internal.InvalidId;
  }
}
