using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// A representation of a musical instrument that can be played in real-time.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Instrument : MonoBehaviour {
    /// Audio source.
    public AudioSource Source { get; private set; } = null;

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

    /// Instrument create callback.
    public event Action OnInstrumentCreate;

    /// Instrument destroy callback.
    public event Action OnInstrumentDestroy;

    /// Returns a control.
    ///
    /// @param controlId Control identifier.
    /// @return Control.
    public Control GetControl(int controlId) {
      return Musician.Internal.Instrument_GetControl(_ptr, controlId);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param Note pitch.
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return _notes.ContainsKey(pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      foreach (var note in _notes) {
        note.Value.Destroy();
        OnNoteOff?.Invoke(note.Key);
        OnNoteOffEvent?.Invoke((float)note.Key);
      }
      _notes.Clear();
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Instrument_SetData(_ptr, dataPtr, size);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Note note;
      if (_notes.TryGetValue(pitch, out note)) {
        note.Destroy();
        _notes.Remove(pitch);
        OnNoteOff?.Invoke(pitch);
        OnNoteOffEvent?.Invoke((float)pitch);
      }
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch, double intensity = 1.0) {
      if (_notes.ContainsKey(pitch)) {
        return;
      }
      Note note = Musician.Internal.Note_Create(_ptr, pitch, intensity);
      if (note != null) {
        _notes.Add(pitch, note);
        OnNoteOn?.Invoke(pitch, intensity);
        OnNoteOnEvent?.Invoke((float)pitch, (float)intensity);
      }
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the pointer.
      public static IntPtr GetPtr(Instrument instrument) {
        return instrument ? instrument._ptr : IntPtr.Zero;
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
      _notes = new Dictionary<double, Note>();
    }

    protected virtual void OnDestroy() {
      Source = null;
      _notes = null;
    }

    protected virtual void OnEnable() {
      Musician.Internal.Instrument_Create(this, ref _ptr);
      OnInstrumentCreate?.Invoke();
      Source?.Play();
    }

    protected virtual void OnDisable() {
      SetAllNotesOff();
      Source?.Stop();
      OnInstrumentDestroy?.Invoke();
      Musician.Internal.Instrument_Destroy(ref _ptr);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Instrument_Process(_ptr, data, channels);
    }

    // Map of notes by their pitches.
    private Dictionary<double, Note> _notes = null;

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
