using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Class that communicates with the native API.
  public static class Musician {
    /// Invalid id.
    public const Int64 InvalidId = -1;

    /// Instrument note off event.
    public delegate void InstrumentNoteOffEvent(Instrument instrument, float notePitch);
    public static event InstrumentNoteOffEvent OnInstrumentNoteOff;

    /// Instrument note on event.
    public delegate void InstrumentNoteOnEvent(Instrument instrument, float notePitch,
                                               float noteIntensity);
    public static event InstrumentNoteOnEvent OnInstrumentNoteOn;

    /// Playback beat event.
    public delegate void PlaybackBeatEvent(double position);
    public static event PlaybackBeatEvent OnPlaybackBeat;

    /// Adds new instrument.
    ///
    /// @param instrument Instrument to add.
    /// @return Instrument id.
    public static Int64 AddInstrument(Instrument instrument) {
      Int64 instrumentId = InvalidId;
      Type instrumentType = instrument.GetType();
      if (instrumentType == typeof(SynthInstrument)) {
        Status status = AddSynthInstrumentNative(Handle, _int64Ptr);
        if (IsOk(status)) {
          instrumentId = Marshal.ReadInt64(_int64Ptr);
        } else {
          Debug.LogError("Failed to add instrument (" + instrument.name + "): " + status);
        }
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      if (instrumentId != InvalidId) {
        _instruments.Add(instrumentId, instrument);
      }
      return instrumentId;
    }

    /// Adds new performer.
    ///
    /// @param performer Performer to add.
    /// @return Performer id.
    public static Int64 AddPerformer(Performer performer) {
      Int64 performerId = InvalidId;
      Status status = AddPerformerNative(Handle, _int64Ptr);
      if (IsOk(status)) {
        performerId = Marshal.ReadInt64(_int64Ptr);
      } else {
        Debug.LogError("Failed to add performer (" + performer.name + "): " + status);
      }
      return performerId;
    }

    /// Returns the playback position.
    ///
    /// @return Playback position in beats.
    public static double GetPlaybackPosition() {
      Status status = GetPlaybackPositionNative(Handle, _doublePtr);
      if (IsOk(status)) {
        return Marshal.PtrToStructure<Double>(_doublePtr);
      } else {
        Debug.LogError("Failed to get playback position: " + status);
      }
      return 0.0;
    }

    /// Returns the playback tempo.
    ///
    /// @return Playback tempo.
    public static double GetPlaybackTempo() {
      Status status = GetPlaybackTempoNative(Handle, _doublePtr);
      if (IsOk(status)) {
        return Marshal.PtrToStructure<Double>(_doublePtr);
      } else {
        Debug.LogError("Failed to get playback tempo: " + status);
      }
      return 0.0;
    }

    /// Returns whether the playback is currently active or not.
    ///
    /// @return True if playing, false otherwise.
    public static bool IsPlaying() {
      Status status = IsPlayingNative(Handle, _booleanPtr);
      if (IsOk(status)) {
        return Marshal.PtrToStructure<Boolean>(_booleanPtr);
      } else {
        Debug.LogError("Failed to get playback state: " + status);
      }
      return false;
    }

    /// Processes the next instrument buffer.
    ///
    /// @param instrument Instrument to process.
    /// @param output Output buffer.
    /// @param numChannels Number of channels.
    public static void ProcessInstrument(Instrument instrument, float[] output, int numChannels) {
      ProcessInstrumentNative(Handle, instrument.Id, AudioSettings.dspTime, output, numChannels,
                              output.Length / numChannels);
    }

    /// Removes instrument.
    ///
    /// @param instrument Instrument to remove.
    public static void RemoveInstrument(Instrument instrument) {
      RemoveInstrumentNative(Handle, instrument.Id);
      _instruments.Remove(instrument.Id);
    }

    /// Removes performer.
    ///
    /// @param performer Performer to remove.
    public static void RemovePerformer(Performer performer) {
      RemovePerformerNative(Handle, performer.Id);
    }

    /// Sets all instrument notes off.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentNotesOff(Instrument instrument) {
      return IsOk(SetAllInstrumentNotesOffNative(Handle, instrument.Id));
    }

    /// Sets all instrument parameters to default value.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentParamsToDefault(Instrument instrument) {
      return IsOk(SetAllInstrumentParamsToDefaultNative(Handle, instrument.Id));
    }

    /// Sets instrument note off.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOff(Instrument instrument, float notePitch) {
      return IsOk(SetInstrumentNoteOffNative(Handle, instrument.Id, notePitch));
    }

    /// Sets instrument note on.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @param noteIntensity Note intensity.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOn(Instrument instrument, float notePitch,
                                           float noteIntensity) {
      return IsOk(SetInstrumentNoteOnNative(Handle, instrument.Id, notePitch, noteIntensity));
    }

    /// Sets instrument parameter value.
    ///
    /// @param instrument Instrument to set.
    /// @param paramId Parameter id.
    /// @param paramValue Parameter value.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParam(Instrument instrument, int paramId, float paramValue) {
      return IsOk(SetInstrumentParamNative(Handle, instrument.Id, paramId, paramValue));
    }

    /// Sets instrument parameter value to default.
    ///
    /// @param instrument Instrument to set.
    /// @param paramId Parameter id.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParamToDefault(Instrument instrument, int paramId) {
      return IsOk(SetInstrumentParamToDefaultNative(Handle, instrument.Id, paramId));
    }

    /// Sets playback position.
    ///
    /// @param position Playback position in beats.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackPosition(double position) {
      return IsOk(SetPlaybackPositionNative(Handle, position));
    }

    /// Sets playback tempo.
    ///
    /// @param tempo Playback tempo in BPM.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackTempo(double tempo) {
      return IsOk(SetPlaybackTempoNative(Handle, tempo));
    }

    /// Starts the playback.
    ///
    /// @return True if success, false otherwise.
    public static bool StartPlayback() {
      return IsOk(StartPlaybackNative(Handle));
    }

    /// Stops the playback.
    ///
    /// @return True if success, false otherwise.
    public static bool StopPlayback() {
      return IsOk(StopPlaybackNative(Handle));
    }

    /// Updates performer.
    ///
    /// @param performer Performer to update.
    /// TODO(#85): This is a POC implementation only, also missing begin/end position setters.
    public static void UpdatePerformer(Performer performer) {
      SetPerformerBeginOffsetNative(Handle, performer.Id, performer.BeginOffset);
      SetPerformerLoopNative(Handle, performer.Id, performer.Loop);
      SetPerformerLoopBeginOffsetNative(Handle, performer.Id, performer.LoopBeginOffset);
      SetPerformerLoopLengthNative(Handle, performer.Id, performer.LoopLength);
      RemoveAllPerformerNotesNative(Handle, performer.Id);
      foreach (var performerNote in performer.Notes) {
        float pitch = (float)(performer.RootNote + performerNote.note.Pitch - 69) / 12.0f;
        AddPerformerNoteNative(Handle, performer.Id, performerNote.position,
                               performerNote.note.Duration, pitch, performerNote.note.Intensity,
                               _int64Ptr);
      }
      foreach (var instrument in performer.Instruments) {
        if (instrument != null) {
          AddPerformerInstrumentNative(Handle, performer.Id, instrument.Id);
        }
      }
    }

    // Singleton handle.
    private static IntPtr Handle {
      get {
        if (_isShuttingDown) {
          return IntPtr.Zero;
        }
        if (_handle == IntPtr.Zero) {
          var instance = new GameObject() { hideFlags = HideFlags.HideAndDontSave }
                             .AddComponent<MusicianInternal>();
          GameObject.DontDestroyOnLoad(instance.gameObject);
          if (_handle != IntPtr.Zero) {
            _booleanPtr = Marshal.AllocHGlobal(Marshal.SizeOf<Boolean>());
            _doublePtr = Marshal.AllocHGlobal(Marshal.SizeOf<Double>());
            _int64Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int64>());
          } else {
            Debug.LogError("Could not initialize BarelyMusician.");
            GameObject.DestroyImmediate(instance.gameObject);
          }
        }
        return _handle;
      }
    }
    private static IntPtr _handle = IntPtr.Zero;

    // Boolean type pointer.
    private static IntPtr _booleanPtr = IntPtr.Zero;

    // Double type pointer.
    private static IntPtr _doublePtr = IntPtr.Zero;

    // Int64 type pointer.
    private static IntPtr _int64Ptr = IntPtr.Zero;

    // List of instruments.
    private static Dictionary<Int64, Instrument> _instruments = new Dictionary<Int64, Instrument>();

    // Denotes if the system is shutting down to avoid re-initialization.
    private static bool _isShuttingDown = false;

    // Internal component to update the native state.
    private class MusicianInternal : MonoBehaviour {
      // Instrument note off callback.
      private delegate void InstrumentNoteOffCallback(Int64 instrumentId, float notePitch);
      private InstrumentNoteOffCallback _instrumentNoteOffCallback = null;

      // Instrument note on callback.
      private delegate void InstrumentNoteOnCallback(Int64 instrument_id, float notePitch,
                                                     float noteIntensity);
      private InstrumentNoteOnCallback _instrumentNoteOnCallback = null;

      // Playback beat callback.
      private delegate void PlaybackBeatCallback(double position);
      private PlaybackBeatCallback _playbackBeatCallback = null;

      private void Awake() {
        AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
        _handle = CreateNative(AudioSettings.outputSampleRate);
        _instrumentNoteOffCallback = delegate(Int64 instrumentId, float notePitch) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(instrumentId, out instrument)) {
            OnInstrumentNoteOff?.Invoke(instrument, notePitch);
          } else {
            Debug.LogWarning("Instrument does not exist: " + instrumentId);
          }
        };
        SetInstrumentNoteOffCallbackNative(
            _handle, Marshal.GetFunctionPointerForDelegate(_instrumentNoteOffCallback));
        _instrumentNoteOnCallback =
            delegate(Int64 instrumentId, float notePitch, float noteIntensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(instrumentId, out instrument)) {
            OnInstrumentNoteOn?.Invoke(instrument, notePitch, noteIntensity);
          } else {
            Debug.LogWarning("Instrument does not exist: " + instrumentId);
          }
        };
        SetInstrumentNoteOnCallbackNative(
            _handle, Marshal.GetFunctionPointerForDelegate(_instrumentNoteOnCallback));
        _playbackBeatCallback = delegate(double position) {
          OnPlaybackBeat?.Invoke(position);
        };
        SetPlaybackBeatCallbackNative(_handle,
                                      Marshal.GetFunctionPointerForDelegate(_playbackBeatCallback));
      }

      private void OnDestroy() {
        AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
        DestroyNative(_handle);
        _handle = IntPtr.Zero;
        Marshal.FreeHGlobal(_booleanPtr);
        Marshal.FreeHGlobal(_doublePtr);
        Marshal.FreeHGlobal(_int64Ptr);
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        foreach (var instrument in _instruments.Values) {
          SetAllInstrumentNotesOffNative(_handle, instrument.Id);
        }
        GameObject.Destroy(gameObject);
      }

      private void OnAudioConfigurationChanged(bool deviceWasChanged) {
        SetSampleRateNative(_handle, AudioSettings.outputSampleRate);
        foreach (var instrument in _instruments.Values) {
          instrument.Source?.Play();
        }
      }

      private void LateUpdate() {
        double lookahead = 2.0 * (double)Time.smoothDeltaTime;
        double updateTime = AudioSettings.dspTime + lookahead;
        UpdateNative(_handle, updateTime);
      }
    }

    // Status codes.
    private enum Status {
      // Success.
      Ok = 0,
      // Invalid argument error.
      InvalidArgument = 1,
      // Not found error.
      NotFound = 2,
      // Already exists error.
      AlreadyExists = 3,
      // Failed precondition error.
      FailedPrecondition = 4,
      // Unimplemented error.
      Unimplemented = 5,
      // Internal error.
      Internal = 6,
      // Unknown error.
      Unknown = 7,
    }

    // Returns whether given |status| is ok.
    private static bool IsOk(Status status) {
      return (status == Status.Ok);
    }

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformer")]
    private static extern Status AddPerformerNative(IntPtr handle, IntPtr performerIdPtr);

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformerInstrument")]
    private static extern Status AddPerformerInstrumentNative(IntPtr handle, Int64 performerId,
                                                              Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformerNote")]
    private static extern Status AddPerformerNoteNative(IntPtr handle, Int64 performerId,
                                                        double notePosition, double noteDuration,
                                                        float notePitch, float noteIntensity,
                                                        IntPtr noteIdPtr);

    [DllImport(pluginName, EntryPoint = "BarelyAddSynthInstrument")]
    private static extern Status AddSynthInstrumentNative(IntPtr handle, IntPtr instrumentIdPtr);

    [DllImport(pluginName, EntryPoint = "BarelyCreate")]
    private static extern IntPtr CreateNative(Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyDestroy")]
    private static extern Status DestroyNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyGetPlaybackPosition")]
    private static extern Status GetPlaybackPositionNative(IntPtr handle, IntPtr positionPtr);

    [DllImport(pluginName, EntryPoint = "BarelyGetPlaybackTempo")]
    private static extern Status GetPlaybackTempoNative(IntPtr handle, IntPtr tempoPtr);

    [DllImport(pluginName, EntryPoint = "BarelyIsPlaying")]
    private static extern Status IsPlayingNative(IntPtr handle, IntPtr isPlayingPtr);

    [DllImport(pluginName, EntryPoint = "BarelyProcessInstrument")]
    private static extern Status ProcessInstrumentNative(IntPtr handle, Int64 instrumentId,
                                                         double timestamp, [In, Out] float[] output,
                                                         Int32 numChannels, Int32 numFrames);

    [DllImport(pluginName, EntryPoint = "BarelyRemoveAllPerformerNotes")]
    private static extern Status RemoveAllPerformerNotesNative(IntPtr handle, Int64 performerId);

    [DllImport(pluginName, EntryPoint = "BarelyRemoveInstrument")]
    private static extern Status RemoveInstrumentNative(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyRemovePerformer")]
    private static extern Status RemovePerformerNative(IntPtr handle, Int64 performerId);

    [DllImport(pluginName, EntryPoint = "BarelyRemovePerformerNote")]
    private static extern Status RemovePerformerNoteNative(IntPtr handle, Int64 performerId,
                                                           Int64 noteId);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentNotesOff")]
    private static extern Status SetAllInstrumentNotesOffNative(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentParamsToDefault")]
    private static extern Status SetAllInstrumentParamsToDefaultNative(IntPtr handle,
                                                                       Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOffCallback")]
    private static extern Status SetInstrumentNoteOffCallbackNative(IntPtr handle,
                                                                    IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOff")]
    private static extern Status SetInstrumentNoteOffNative(IntPtr handle, Int64 instrumentId,
                                                            float notePitch);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOnCallback")]
    private static extern Status SetInstrumentNoteOnCallbackNative(IntPtr handle,
                                                                   IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOn")]
    private static extern Status SetInstrumentNoteOnNative(IntPtr handle, Int64 instrumentId,
                                                           float notePitch, float noteIntensity);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParam")]
    private static extern Status SetInstrumentParamNative(IntPtr handle, Int64 instrumentId,
                                                          Int32 paramId, float paramValue);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParamToDefault")]
    private static extern Status SetInstrumentParamToDefaultNative(IntPtr handle,
                                                                   Int64 instrumentId,
                                                                   Int32 paramId);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerBeginOffset")]
    private static extern Status SetPerformerBeginOffsetNative(IntPtr handle, Int64 performerId,
                                                               double beginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerBeginPosition")]
    private static extern Status SetPerformerBeginPositionNative(IntPtr handle, Int64 performerId,
                                                                 IntPtr beginPosition);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerEndPosition")]
    private static extern Status SetPerformerEndPositionNative(IntPtr handle, Int64 performerId,
                                                               IntPtr endPosition);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoop")]
    private static extern Status SetPerformerLoopNative(IntPtr handle, Int64 performerId,
                                                        bool loop);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoopBeginOffset")]
    private static extern Status SetPerformerLoopBeginOffsetNative(IntPtr handle, Int64 performerId,
                                                                   double loopBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoopLength")]
    private static extern Status SetPerformerLoopLengthNative(IntPtr handle, Int64 performerId,
                                                              double loopLength);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackBeatCallback")]
    private static extern Status SetPlaybackBeatCallbackNative(IntPtr handle,
                                                               IntPtr playbackBeatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackPosition")]
    private static extern Status SetPlaybackPositionNative(IntPtr handle, double position);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackTempo")]
    private static extern Status SetPlaybackTempoNative(IntPtr handle, double tempo);

    [DllImport(pluginName, EntryPoint = "BarelySetSampleRate")]
    private static extern Status SetSampleRateNative(IntPtr handle, Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyStartPlayback")]
    private static extern Status StartPlaybackNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyStopPlayback")]
    private static extern Status StopPlaybackNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyUpdate")]
    private static extern Status UpdateNative(IntPtr handle, double timestamp);
  }
}
