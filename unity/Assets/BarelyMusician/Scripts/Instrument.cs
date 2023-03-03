using System;
using UnityEngine;

namespace Barely {
  /// A representation of a musical instrument that can be played in real-time.
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

    [Serializable]
    public class NoteControlEvent : UnityEngine.Events.UnityEvent<float, int, float> {}
    public NoteControlEvent OnNoteControlEvent;

    /// Note off event callback.
    ///
    /// @param pitch Note pitch.
    public delegate void NoteOffEventCallback(double pitch);
    public event NoteOffEventCallback OnNoteOff;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on event callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public delegate void NoteOnEventCallback(double pitch, double intensity);
    public event NoteOnEventCallback OnNoteOn;

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
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Instrument_SetData(_id, dataPtr, size);
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

    /// Class that wraps the internal api.
    public static class Internal {
      /// Internal control event callback.
      public static void OnControlEvent(Instrument instrument, int index, double value) {
        instrument.OnControl?.Invoke(index, value);
        instrument.OnControlEvent?.Invoke(index, (float)value);
      }

      /// Internal note control event callback.
      public static void OnNoteControlEvent(Instrument instrument, double pitch, int index,
                                            double value) {
        instrument.OnNoteControl?.Invoke(pitch, index, value);
        instrument.OnNoteControlEvent?.Invoke((float)pitch, index, (float)value);
      }

      public static void OnNoteOffEvent(Instrument instrument, double pitch) {
        instrument.OnNoteOff?.Invoke(pitch);
        instrument.OnNoteOffEvent?.Invoke((float)pitch);
      }

      /// Internal note on event callback.
      public static void OnNoteOnEvent(Instrument instrument, double pitch, double intensity) {
        instrument.OnNoteOn?.Invoke(pitch, intensity);
        instrument.OnNoteOnEvent?.Invoke((float)pitch, (float)intensity);
      }
    }

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
      Source.clip = AudioClip.Create("Ones", 64, 1, AudioSettings.outputSampleRate, false);
      float[] ones = new float[64];
      for (int i = 0; i < ones.Length; ++i) {
        ones[i] = 1.0f;
      }
      Source.clip.SetData(ones, 0);
      Source.loop = true;
    }

    protected virtual void OnDestroy() {
      Source = null;
    }

    protected virtual void OnEnable() {
      Musician.Internal.Instrument_Create(this, ref _id);
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      Musician.Internal.Instrument_Destroy(ref _id);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_id, data, channels);
    }

    // Identifier.
    private Int64 _id = Musician.Internal.InvalidId;
  }
}
