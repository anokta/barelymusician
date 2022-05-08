using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Parameter definition.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct ParameterDefinition {
    /// Default value.
    double defaultValue;

    /// Minimum value.
    double minValue;

    /// Maximum value.
    double maxValue;
  }

  // Instrument definition.
  [Serializable]
  [StructLayout(LayoutKind.Sequential)]
  public struct InstrumentDefinition {
    // TODO(#105): Need to pass the array size for Marshal `SizeParamIndex`.
    public delegate void ProcessCallback(IntPtr state,
                                         [MarshalAs(UnmanagedType.LPArray,
                                                    SizeParamIndex = 3)] double[] output,
                                         Int32 numOutputChannels, Int32 numOutputFrames);
    public delegate void SetDataCallback(IntPtr state, byte[] data, Int32 size);
    public delegate void SetNoteOffCallback(IntPtr state, double pitch);
    public delegate void SetNoteOnCallback(IntPtr state, double pitch, double intensity);
    public delegate void SetParameterCallback(IntPtr state, Int32 index, double value);

    public InstrumentDefinition(ProcessCallback processCallback, SetDataCallback setDataCallback,
                                SetNoteOffCallback setNoteOffCallback,
                                SetNoteOnCallback setNoteOnCallback,
                                SetParameterCallback setParameterCallback,
                                ParameterDefinition[] parameterDefinitions) {
      _createCallback = IntPtr.Zero;
      _destroyCallback = IntPtr.Zero;
      _processCallback = Marshal.GetFunctionPointerForDelegate(processCallback);
      _setDataCallback = Marshal.GetFunctionPointerForDelegate(setDataCallback);
      _setNoteOffCallback = Marshal.GetFunctionPointerForDelegate(setNoteOffCallback);
      _setNoteOnCallback = Marshal.GetFunctionPointerForDelegate(setNoteOnCallback);
      _setParameterCallback = Marshal.GetFunctionPointerForDelegate(setParameterCallback);
      _parameterDefinitions = parameterDefinitions;
      _numParameterDefinitions = (parameterDefinitions != null) ? parameterDefinitions.Length : 0;
    }

    // Create callback.
    private IntPtr _createCallback;

    // Destroy callback.
    private IntPtr _destroyCallback;

    // Process callback.
    private IntPtr _processCallback;

    // Set data callback.
    private IntPtr _setDataCallback;

    // Set note off callback.
    private IntPtr _setNoteOffCallback;

    // Set note on callback.
    private IntPtr _setNoteOnCallback;

    // Set parameter callback.
    private IntPtr _setParameterCallback;

    // List of parameter definitions.
    private ParameterDefinition[] _parameterDefinitions;

    // Number of parameter definitions.
    private Int32 _numParameterDefinitions;
  }

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
    public delegate void NoteOffCallback(double pitch, double dspTime);
    public event NoteOffCallback OnNoteOff;
    private NoteOffCallback _noteOffCallback = null;

    [Serializable]
    public class NoteOffEvent : UnityEngine.Events.UnityEvent<double> {}
    public NoteOffEvent NoteOff;

    /// Note on callback.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    /// @param dspTime Note on time in seconds.
    public delegate void NoteOnCallback(double pitch, double intensity, double dspTime);
    public event NoteOnCallback OnNoteOn;
    private NoteOnCallback _noteOnCallback = null;

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

    /// Returns parameter definition.
    ///
    /// @param index Parameter index.
    /// @return Parameter definition.
    public ParameterDefinition GetParameterDefinition(int index) {
      return Musician.Native.Instrument_GetParameterDefinition(this, index);
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
    /// @param value Parameter slope in value change per second.
    public void SetParameter(int index, double value, double slope = 0.0) {
      Musician.Native.Instrument_SetParameter(this, index, value, slope);
    }

    /// Starts playing note.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void StartNote(double pitch, double intensity = 1.0) {
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
