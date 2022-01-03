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
        Status status = CreateSynthInstrumentNative(Api, _int64Ptr);
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
      Status status = AddPerformerNative(Api, _int64Ptr);
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
      Status status = GetPlaybackPositionNative(Api, _doublePtr);
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
      Status status = GetPlaybackTempoNative(Api, _doublePtr);
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
      Status status = IsPlayingNative(Api, _booleanPtr);
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
      ProcessInstrumentNative(Api, instrument.Id, AudioSettings.dspTime, output, numChannels,
                              output.Length / numChannels);
    }

    /// Removes instrument.
    ///
    /// @param instrument Instrument to remove.
    public static void RemoveInstrument(Instrument instrument) {
      DestroyInstrumentNative(Api, instrument.Id);
      _instruments.Remove(instrument.Id);
    }

    /// Removes performer.
    ///
    /// @param performer Performer to remove.
    public static void RemovePerformer(Performer performer) {
      RemovePerformerNative(Api, performer.Id);
    }

    /// Sets all instrument notes off.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentNotesOff(Instrument instrument) {
      return IsOk(SetAllInstrumentNotesOffNative(Api, instrument.Id));
    }

    /// Sets all instrument parameters to default value.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentParamsToDefault(Instrument instrument) {
      return IsOk(SetAllInstrumentParamsToDefaultNative(Api, instrument.Id));
    }

    /// Sets instrument note off.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOff(Instrument instrument, float notePitch) {
      return IsOk(SetInstrumentNoteOffNative(Api, instrument.Id, notePitch));
    }

    /// Sets instrument note on.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @param noteIntensity Note intensity.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOn(Instrument instrument, float notePitch,
                                           float noteIntensity) {
      return IsOk(SetInstrumentNoteOnNative(Api, instrument.Id, notePitch, noteIntensity));
    }

    /// Sets instrument parameter value.
    ///
    /// @param instrument Instrument to set.
    /// @param paramIndex Parameter index.
    /// @param paramValue Parameter value.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParam(Instrument instrument, int paramIndex, float paramValue) {
      return IsOk(SetInstrumentParamNative(Api, instrument.Id, paramIndex, paramValue));
    }

    /// Sets instrument parameter value to default.
    ///
    /// @param instrument Instrument to set.
    /// @param paramIndex Parameter index.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParamToDefault(Instrument instrument, int paramIndex) {
      return IsOk(SetInstrumentParamToDefaultNative(Api, instrument.Id, paramIndex));
    }

    /// Sets playback position.
    ///
    /// @param position Playback position in beats.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackPosition(double position) {
      return IsOk(SetPlaybackPositionNative(Api, position));
    }

    /// Sets playback tempo.
    ///
    /// @param tempo Playback tempo in BPM.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackTempo(double tempo) {
      return IsOk(SetPlaybackTempoNative(Api, tempo));
    }

    /// Starts the playback.
    ///
    /// @return True if success, false otherwise.
    public static bool StartPlayback() {
      return IsOk(StartPlaybackNative(Api));
    }

    /// Stops the playback.
    ///
    /// @return True if success, false otherwise.
    public static bool StopPlayback() {
      return IsOk(StopPlaybackNative(Api));
    }

    /// Updates performer.
    ///
    /// @param performer Performer to update.
    /// TODO(#85): This is a POC implementation only, also missing begin/end position setters.
    public static void UpdatePerformer(Performer performer) {
      SetPerformerBeginOffsetNative(Api, performer.Id, performer.BeginOffset);
      SetPerformerLoopNative(Api, performer.Id, performer.Loop);
      SetPerformerLoopBeginOffsetNative(Api, performer.Id, performer.LoopBeginOffset);
      SetPerformerLoopLengthNative(Api, performer.Id, performer.LoopLength);

      RemoveAllPerformerNotesNative(Api, performer.Id);
      foreach (var performerNote in performer.Notes) {
        float pitch = (float)(performer.RootNote + performerNote.note.Pitch - 69) / 12.0f;
        AddPerformerNoteNative(Api, performer.Id, performerNote.position,
                               performerNote.note.Duration, pitch, performerNote.note.Intensity,
                               _int64Ptr);
      }
      SetPerformerInstrumentNative(Api, performer.Id,
                                   performer.Instrument ? performer.Instrument.Id : InvalidId);
    }

    // Singleton API.
    private static IntPtr Api {
      get {
        if (_isShuttingDown) {
          return IntPtr.Zero;
        }
        if (_api == IntPtr.Zero) {
          var instance = new GameObject() { hideFlags = HideFlags.HideAndDontSave }
                             .AddComponent<MusicianInternal>();
          GameObject.DontDestroyOnLoad(instance.gameObject);
          if (_api != IntPtr.Zero) {
            _booleanPtr = Marshal.AllocHGlobal(Marshal.SizeOf<Boolean>());
            _doublePtr = Marshal.AllocHGlobal(Marshal.SizeOf<Double>());
            _int64Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int64>());
          } else {
            Debug.LogError("Could not initialize BarelyMusician.");
            GameObject.DestroyImmediate(instance.gameObject);
          }
        }
        return _api;
      }
    }
    private static IntPtr _api = IntPtr.Zero;

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
        _api = CreateApiNative(AudioSettings.outputSampleRate);
        _instrumentNoteOffCallback = delegate(Int64 instrumentId, float notePitch) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(instrumentId, out instrument)) {
            OnInstrumentNoteOff?.Invoke(instrument, notePitch);
          } else {
            Debug.LogWarning("Instrument does not exist: " + instrumentId);
          }
        };
        SetInstrumentNoteOffCallbackNative(
            _api, Marshal.GetFunctionPointerForDelegate(_instrumentNoteOffCallback));
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
            _api, Marshal.GetFunctionPointerForDelegate(_instrumentNoteOnCallback));
        _playbackBeatCallback = delegate(double position) {
          OnPlaybackBeat?.Invoke(position);
        };
        SetPlaybackBeatCallbackNative(_api,
                                      Marshal.GetFunctionPointerForDelegate(_playbackBeatCallback));
      }

      private void OnDestroy() {
        AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
        DestroyApiNative(_api);
        _api = IntPtr.Zero;
        Marshal.FreeHGlobal(_booleanPtr);
        Marshal.FreeHGlobal(_doublePtr);
        Marshal.FreeHGlobal(_int64Ptr);
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        foreach (var instrument in _instruments.Values) {
          SetAllInstrumentNotesOffNative(_api, instrument.Id);
        }
        GameObject.Destroy(gameObject);
      }

      private void OnAudioConfigurationChanged(bool deviceWasChanged) {
        SetSampleRateNative(_api, AudioSettings.outputSampleRate);
        foreach (var instrument in _instruments.Values) {
          instrument.Source?.Play();
        }
      }

      private void LateUpdate() {
        double lookahead = 2.0 * (double)Time.smoothDeltaTime;
        double updateTime = AudioSettings.dspTime + lookahead;
        UpdateNative(_api, updateTime);
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
    private static extern Status AddPerformerNative(IntPtr api, IntPtr performerIdPtr);

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformerNote")]
    private static extern Status AddPerformerNoteNative(IntPtr api, Int64 performerId,
                                                        double notePosition, double noteDuration,
                                                        float notePitch, float noteIntensity,
                                                        IntPtr noteIdPtr);

    [DllImport(pluginName, EntryPoint = "BarelyCreateApi")]
    private static extern IntPtr CreateApiNative(Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyCreateSynthInstrument")]
    private static extern Status CreateSynthInstrumentNative(IntPtr api, IntPtr instrumentIdPtr);

    [DllImport(pluginName, EntryPoint = "BarelyDestroyApi")]
    private static extern Status DestroyApiNative(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyDestroyInstrument")]
    private static extern Status DestroyInstrumentNative(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyGetPlaybackPosition")]
    private static extern Status GetPlaybackPositionNative(IntPtr api, IntPtr positionPtr);

    [DllImport(pluginName, EntryPoint = "BarelyGetPlaybackTempo")]
    private static extern Status GetPlaybackTempoNative(IntPtr api, IntPtr tempoPtr);

    [DllImport(pluginName, EntryPoint = "BarelyIsPlaying")]
    private static extern Status IsPlayingNative(IntPtr api, IntPtr isPlayingPtr);

    [DllImport(pluginName, EntryPoint = "BarelyProcessInstrument")]
    private static extern Status ProcessInstrumentNative(IntPtr api, Int64 instrumentId,
                                                         double timestamp, [In, Out] float[] output,
                                                         Int32 numChannels, Int32 numFrames);

    [DllImport(pluginName, EntryPoint = "BarelyRemoveAllPerformerNotes")]
    private static extern Status RemoveAllPerformerNotesNative(IntPtr api, Int64 performerId);

    [DllImport(pluginName, EntryPoint = "BarelyRemovePerformer")]
    private static extern Status RemovePerformerNative(IntPtr api, Int64 performerId);

    [DllImport(pluginName, EntryPoint = "BarelyRemovePerformerNote")]
    private static extern Status RemovePerformerNoteNative(IntPtr api, Int64 performerId,
                                                           Int64 noteId);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentNotesOff")]
    private static extern Status SetAllInstrumentNotesOffNative(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentParamsToDefault")]
    private static extern Status SetAllInstrumentParamsToDefaultNative(IntPtr api,
                                                                       Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOffCallback")]
    private static extern Status SetInstrumentNoteOffCallbackNative(IntPtr api,
                                                                    IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOff")]
    private static extern Status SetInstrumentNoteOffNative(IntPtr api, Int64 instrumentId,
                                                            float notePitch);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOnCallback")]
    private static extern Status SetInstrumentNoteOnCallbackNative(IntPtr api,
                                                                   IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOn")]
    private static extern Status SetInstrumentNoteOnNative(IntPtr api, Int64 instrumentId,
                                                           float notePitch, float noteIntensity);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParam")]
    private static extern Status SetInstrumentParamNative(IntPtr api, Int64 instrumentId,
                                                          Int32 paramIndex, float paramValue);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParamToDefault")]
    private static extern Status SetInstrumentParamToDefaultNative(IntPtr api, Int64 instrumentId,
                                                                   Int32 paramIndex);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerBeginOffset")]
    private static extern Status SetPerformerBeginOffsetNative(IntPtr api, Int64 performerId,
                                                               double beginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerBeginPosition")]
    private static extern Status SetPerformerBeginPositionNative(IntPtr api, Int64 performerId,
                                                                 IntPtr beginPosition);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerEndPosition")]
    private static extern Status SetPerformerEndPositionNative(IntPtr api, Int64 performerId,
                                                               IntPtr endPosition);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerInstrument")]
    private static extern Status SetPerformerInstrumentNative(IntPtr api, Int64 performerId,
                                                              Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoop")]
    private static extern Status SetPerformerLoopNative(IntPtr api, Int64 performerId, bool loop);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoopBeginOffset")]
    private static extern Status SetPerformerLoopBeginOffsetNative(IntPtr api, Int64 performerId,
                                                                   double loopBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoopLength")]
    private static extern Status SetPerformerLoopLengthNative(IntPtr api, Int64 performerId,
                                                              double loopLength);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackBeatCallback")]
    private static extern Status SetPlaybackBeatCallbackNative(IntPtr api,
                                                               IntPtr playbackBeatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackPosition")]
    private static extern Status SetPlaybackPositionNative(IntPtr api, double position);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackTempo")]
    private static extern Status SetPlaybackTempoNative(IntPtr api, double tempo);

    [DllImport(pluginName, EntryPoint = "BarelySetSampleRate")]
    private static extern Status SetSampleRateNative(IntPtr api, Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyStartPlayback")]
    private static extern Status StartPlaybackNative(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyStopPlayback")]
    private static extern Status StopPlaybackNative(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyUpdate")]
    private static extern Status UpdateNative(IntPtr api, double timestamp);
  }
}
