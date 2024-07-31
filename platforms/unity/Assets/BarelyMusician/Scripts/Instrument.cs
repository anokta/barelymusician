﻿using System;
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

    /// Returns a control value.
    ///
    /// @param controlId Control identifier.
    /// @return Control value.
    public double GetControl(int controlId) {
      return Musician.Internal.Instrument_GetControl(_ptr, controlId);
    }

    /// Returns a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param controlId Control identifier.
    /// @return Control value.
    public double GetNoteControl(double pitch, int controlId) {
      return Musician.Internal.Instrument_GetNoteControl(_ptr, pitch, controlId);
    }

    /// Returns whether a note is on or not.
    ///
    /// @param Note pitch.
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Instrument_IsNoteOn(_ptr, pitch);
    }

    /// Resets a control value.
    ///
    /// @param controlId Control identifier.
    public void ResetControl(int controlId) {
      Musician.Internal.Instrument_ResetControl(_ptr, controlId);
    }

    /// Resets a note control value.
    ///
    /// @param pitch Note pitch.
    /// @param controlId Note control identifier.
    public void ResetNoteControl(double pitch, int controlId) {
      Musician.Internal.Instrument_ResetNoteControl(_ptr, pitch, controlId);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Instrument_SetAllNotesOff(_ptr);
    }

    /// Sets a control value.
    ///
    /// @param controlId Control identifier.
    /// @param value Control value.
    public void SetControl(int controlId, double value) {
      Musician.Internal.Instrument_SetControl(_ptr, controlId, value);
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
    /// @param pitch Note pitch.
    /// @param controlId Note control identifier.
    /// @param value Note control value.
    public void SetNoteControl(double pitch, int controlId, double value) {
      Musician.Internal.Instrument_SetNoteControl(_ptr, pitch, controlId, value);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Internal.Instrument_SetNoteOff(_ptr, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch, double intensity = 1.0) {
      Musician.Internal.Instrument_SetNoteOn(_ptr, pitch, intensity);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the handle.
      public static IntPtr GetHandle(Instrument instrument) {
        return instrument ? instrument._ptr : IntPtr.Zero;
      }

      /// Internal note off event callback.
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
