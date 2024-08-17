using System;
using UnityEngine;

namespace Barely {
  /// A representation of a musical instrument that can be played in real-time.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    /// Control event callback.
    ///
    /// @param id Control identifier.
    /// @param control Control value.
    public delegate void ControlEventCallback(int id, double value);
    public event ControlEventCallback OnControl;

    [Serializable]
    public class ControlEvent : UnityEngine.Events.UnityEvent<int, float> {}
    public ControlEvent OnControlEvent;

    /// Note control event callback.
    ///
    /// @param note Note value.
    /// @param id Note control identifier.
    /// @param control Note control value.
    public delegate void NoteControlEventCallback(double note, int id, double value);
    public event NoteControlEventCallback OnNoteControl;

    [Serializable]
    public class NoteControlEvent : UnityEngine.Events.UnityEvent<float, int, float> {}
    public NoteControlEvent OnNoteControlEvent;

    /// Note off event callback.
    ///
    /// @param note Note value.
    public delegate void NoteOffEventCallback(double note);
    public event NoteOffEventCallback OnNoteOff;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<float> {}
    public NoteOffEvent OnNoteOffEvent;

    /// Note on event callback.
    ///
    /// @param note Note value.
    /// @param intensity Note intensity.
    public delegate void NoteOnEventCallback(double note, double intensity);
    public event NoteOnEventCallback OnNoteOn;

    [Serializable]
    public class NoteOnEvent : UnityEngine.Events.UnityEvent<float, float> {}
    public NoteOnEvent OnNoteOnEvent;

    /// Instrument create callback.
    public event Action OnInstrumentCreate;

    /// Instrument destroy callback.
    public event Action OnInstrumentDestroy;

    /// Audio source.
    public AudioSource Source { get; private set; } = null;

    /// Returns a control value.
    ///
    /// @param id Control identifier.
    /// @return Control value.
    public double GetControl(int id) {
      return Musician.Internal.Instrument_GetControl(_ptr, id);
    }

    /// Returns a note control value.
    ///
    /// @param note Note value.
    /// @param id Control identifier.
    /// @return Control value.
    public double GetNoteControl(double note, int id) {
      return Musician.Internal.Instrument_GetNoteControl(_ptr, note, id);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param Note value.
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double note) {
      return Musician.Internal.Instrument_IsNoteOn(_ptr, note);
    }

    /// Resets a control value.
    ///
    /// @param id Control identifier.
    public void ResetControl(int id) {
      Musician.Internal.Instrument_ResetControl(_ptr, id);
    }

    /// Resets a note control value.
    ///
    /// @param note Note value.
    /// @param id Note control identifier.
    public void ResetNoteControl(double note, int id) {
      Musician.Internal.Instrument_ResetNoteControl(_ptr, note, id);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_ptr);
    }

    /// Sets a control value.
    ///
    /// @param id Control identifier.
    /// @param value Control value.
    public void SetControl(int id, double value) {
      Musician.Internal.Instrument_SetControl(_ptr, id, value);
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Instrument_SetData(_ptr, dataPtr, size);
    }

    /// Sets a note control value.
    ///
    /// @param note Note value.
    /// @param id Note control identifier.
    /// @param value Note control value.
    public void SetNoteControl(double note, int id, double value) {
      Musician.Internal.Instrument_SetNoteControl(_ptr, note, id, value);
    }

    /// Sets a note off.
    ///
    /// @param note Note value.
    public void SetNoteOff(double note) {
      Musician.Internal.Instrument_SetNoteOff(_ptr, note);
    }

    /// Sets a note on.
    ///
    /// @param note Note value.
    /// @param intensity Note intensity.
    public void SetNoteOn(double note, double intensity = 1.0) {
      Musician.Internal.Instrument_SetNoteOn(_ptr, note, intensity);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the pointer.
      public static IntPtr GetPtr(Instrument instrument) {
        return instrument ? instrument._ptr : IntPtr.Zero;
      }

      /// Internal control event callback.
      public static void OnControlEvent(Instrument instrument, int id, double value) {
        instrument.OnControl?.Invoke(id, value);
        instrument.OnControlEvent?.Invoke(id, (float)value);
      }

      /// Internal note control event callback.
      public static void OnNoteControlEvent(Instrument instrument, double note, int id,
                                            double value) {
        instrument.OnNoteControl?.Invoke(note, id, value);
        instrument.OnNoteControlEvent?.Invoke((float)note, id, (float)value);
      }

      /// Internal note off event callback.
      public static void OnNoteOffEvent(Instrument instrument, double note) {
        instrument.OnNoteOff?.Invoke(note);
        instrument.OnNoteOffEvent?.Invoke((float)note);
      }

      /// Internal note on event callback.
      public static void OnNoteOnEvent(Instrument instrument, double note, double intensity) {
        instrument.OnNoteOn?.Invoke(note, intensity);
        instrument.OnNoteOnEvent?.Invoke((float)note, (float)intensity);
      }
    }

    protected virtual void Awake() {
      Source = GetComponent<AudioSource>();
      Source.clip = AudioClip.Create("[DO NOT EDIT]", 64, 1, AudioSettings.outputSampleRate, false);
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
      Musician.Internal.Instrument_Create(this, ref _ptr);
      OnInstrumentCreate?.Invoke();
      Source?.Play();
    }

    protected virtual void OnDisable() {
      Source?.Stop();
      OnInstrumentDestroy?.Invoke();
      Musician.Internal.Instrument_Destroy(ref _ptr);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_ptr, data, channels);
    }

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
