using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Class that communicates with the native api.
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
        Status status = BarelyExamples_CreateSynthInstrument(Api, _int64Ptr);
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

    /// Adds new sequence.
    ///
    /// @param sequence Sequence to add.
    /// @return Sequence id.
    public static Int64 AddSequence(Sequence sequence) {
      Int64 sequenceId = InvalidId;
      Status status = BarelySequence_Create(Api, _int64Ptr);
      if (IsOk(status)) {
        sequenceId = Marshal.ReadInt64(_int64Ptr);
      } else {
        Debug.LogError("Failed to add sequence (" + sequence.name + "): " + status);
      }
      return sequenceId;
    }

    /// Returns the playback position.
    ///
    /// @return Playback position in beats.
    public static double GetPlaybackPosition() {
      Status status = BarelyTransport_GetPosition(Api, _doublePtr);
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
      Status status = BarelyTransport_GetTempo(Api, _doublePtr);
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
      Status status = BarelyTransport_IsPlaying(Api, _booleanPtr);
      if (IsOk(status)) {
        return Marshal.PtrToStructure<Boolean>(_booleanPtr);
      } else {
        Debug.LogError("Failed to get playback state: " + status);
      }
      return false;
    }

    /// Pauses transport.
    ///
    /// @return True if success, false otherwise.
    public static bool Pause() {
      return IsOk(BarelyTransport_Pause(Api));
    }

    /// Plays transport.
    ///
    /// @return True if success, false otherwise.
    public static bool Play() {
      return IsOk(BarelyTransport_Play(Api));
    }

    /// Processes the next instrument buffer.
    ///
    /// @param instrument Instrument to process.
    /// @param output Output buffer.
    /// @param numChannels Number of channels.
    public static void ProcessInstrument(Instrument instrument, float[] output, int numChannels) {
      BarelyInstrument_Process(Api, instrument.Id, AudioSettings.dspTime, output, numChannels,
                               output.Length / numChannels);
    }

    /// Removes instrument.
    ///
    /// @param instrument Instrument to remove.
    public static void RemoveInstrument(Instrument instrument) {
      BarelyInstrument_Destroy(Api, instrument.Id);
      _instruments.Remove(instrument.Id);
    }

    /// Removes sequence.
    ///
    /// @param sequence Sequence to remove.
    public static void RemoveSequence(Sequence sequence) {
      BarelySequence_Destroy(Api, sequence.Id);
    }

    /// Sets all instrument notes off.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentNotesOff(Instrument instrument) {
      return IsOk(BarelyInstrument_StopAllNotes(Api, instrument.Id));
    }

    /// Sets all instrument parameters to default value.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentParamsToDefault(Instrument instrument) {
      return IsOk(BarelyInstrument_ResetAllParams(Api, instrument.Id));
    }

    /// Sets instrument note off.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOff(Instrument instrument, float notePitch) {
      return IsOk(BarelyInstrument_StopNote(Api, instrument.Id, notePitch));
    }

    /// Sets instrument note on.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @param noteIntensity Note intensity.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOn(Instrument instrument, float notePitch,
                                           float noteIntensity) {
      return IsOk(BarelyInstrument_StartNote(Api, instrument.Id, notePitch, noteIntensity));
    }

    /// Sets instrument parameter value.
    ///
    /// @param instrument Instrument to set.
    /// @param paramIndex Parameter index.
    /// @param paramValue Parameter value.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParam(Instrument instrument, int paramIndex, float paramValue) {
      return IsOk(BarelyInstrument_SetParam(Api, instrument.Id, paramIndex, paramValue));
    }

    /// Sets instrument parameter value to default.
    ///
    /// @param instrument Instrument to set.
    /// @param paramIndex Parameter index.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParamToDefault(Instrument instrument, int paramIndex) {
      return IsOk(BarelyInstrument_ResetParam(Api, instrument.Id, paramIndex));
    }

    /// Sets playback position.
    ///
    /// @param position Playback position in beats.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackPosition(double position) {
      return IsOk(BarelyTransport_SetPosition(Api, position));
    }

    /// Sets playback tempo.
    ///
    /// @param tempo Playback tempo in bpm.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackTempo(double tempo) {
      return IsOk(BarelyTransport_SetTempo(Api, tempo));
    }

    /// Stops transport.
    ///
    /// @return True if success, false otherwise.
    public static bool Stop() {
      return IsOk(BarelyTransport_Stop(Api));
    }

    /// Updates sequence.
    ///
    /// @param sequence Sequence to update.
    /// TODO(#85): This is a POC implementation only.
    public static void UpdateSequence(Sequence sequence) {
      BarelySequence_SetBeginOffset(Api, sequence.Id, sequence.BeginOffset);
      BarelySequence_SetBeginPosition(Api, sequence.Id, sequence.BeginPosition);
      BarelySequence_SetEndPosition(Api, sequence.Id, sequence.EndPosition);
      BarelySequence_SetLooping(Api, sequence.Id, sequence.Loop);
      BarelySequence_SetLoopBeginOffset(Api, sequence.Id, sequence.LoopBeginOffset);
      BarelySequence_SetLoopLength(Api, sequence.Id, sequence.LoopLength);

      BarelySequence_RemoveAllNotes(Api, sequence.Id);
      foreach (var sequenceNote in sequence.Notes) {
        float pitch = (float)(sequence.RootNote + sequenceNote.note.Pitch - 69) / 12.0f;
        AddPerformerNoteNative(Api, sequence.Id, sequenceNote.position, sequenceNote.note.Duration,
                               pitch, sequenceNote.note.Intensity, _int64Ptr);
      }
      BarelySequence_SetInstrument(Api, sequence.Id,
                                   sequence.Instrument ? sequence.Instrument.Id : InvalidId);
    }

    // Singleton api.
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
        _api = BarelyApi_Create(AudioSettings.outputSampleRate);
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
        BarelyApi_Destroy(_api);
        _api = IntPtr.Zero;
        Marshal.FreeHGlobal(_booleanPtr);
        Marshal.FreeHGlobal(_doublePtr);
        Marshal.FreeHGlobal(_int64Ptr);
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        foreach (var instrument in _instruments.Values) {
          BarelyInstrument_StopAllNotes(_api, instrument.Id);
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

    [DllImport(pluginName, EntryPoint = "BarelyApi_Create")]
    private static extern IntPtr BarelyApi_Create(Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyApi_Destroy")]
    private static extern Status BarelyApi_Destroy(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyApi_SetSampleRate")]
    private static extern Status SetSampleRateNative(IntPtr api, Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyApi_Update")]
    private static extern Status UpdateNative(IntPtr api, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyExamples_CreateSynthInstrument")]
    private static extern Status BarelyExamples_CreateSynthInstrument(IntPtr api,
                                                                      IntPtr outInstrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
    private static extern Status BarelyInstrument_Destroy(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
    private static extern Status BarelyInstrument_Process(IntPtr api, Int64 instrumentId,
                                                          double timestamp,
                                                          [In, Out] float[] output,
                                                          Int32 numOutputChannels,
                                                          Int32 numOutputFrames);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllParams")]
    private static extern Status BarelyInstrument_ResetAllParams(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetParam")]
    private static extern Status BarelyInstrument_ResetParam(IntPtr api, Int64 instrumentId,
                                                             Int32 index);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffCallback")]
    private static extern Status BarelyInstrument_SetNoteOffCallback(IntPtr api,
                                                                     IntPtr noteOffCallback);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnCallback")]
    private static extern Status BarelyInstrument_SetNoteOnCallback(IntPtr api,
                                                                    IntPtr noteOnCallback);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetParam")]
    private static extern Status BarelyInstrument_SetParam(IntPtr api, Int64 instrumentId,
                                                           Int32 index, float value);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StartNote")]
    private static extern Status BarelyInstrument_StartNote(IntPtr api, Int64 instrumentId,
                                                            float pitch, float intensity);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopAllNotes")]
    private static extern Status BarelyInstrument_StopAllNotes(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopNote")]
    private static extern Status BarelyInstrument_StopNote(IntPtr api, Int64 instrumentId,
                                                           float pitch);

    [DllImport(pluginName, EntryPoint = "BarelySequence_Create")]
    private static extern Status BarelySequence_Create(IntPtr api, IntPtr outSequenceId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_Destroy")]
    private static extern Status BarelySequence_Destroy(IntPtr api, Int64 sequenceId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_RemoveAllNotes")]
    private static extern Status BarelySequence_RemoveAllNotes(IntPtr api, Int64 sequenceId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_RemoveNote")]
    private static extern Status BarelySequence_RemoveNote(IntPtr api, Int64 sequenceId,
                                                           Int64 noteId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetBeginOffset")]
    private static extern Status BarelySequence_SetBeginOffset(IntPtr api, Int64 sequenceId,
                                                               double beginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetBeginPosition")]
    private static extern Status BarelySequence_SetBeginPosition(IntPtr api, Int64 sequenceId,
                                                                 double beginPosition);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetEndPosition")]
    private static extern Status BarelySequence_SetEndPosition(IntPtr api, Int64 sequenceId,
                                                               double endPosition);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetInstrument")]
    private static extern Status BarelySequence_SetInstrument(IntPtr api, Int64 sequenceId,
                                                              Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetLoopBeginOffset")]
    private static extern Status BarelySequence_SetLoopBeginOffset(IntPtr api, Int64 sequenceId,
                                                                   double loopBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetLoopLength")]
    private static extern Status BarelySequence_SetLoopLength(IntPtr api, Int64 sequenceId,
                                                              double loopLength);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetLooping")]
    private static extern Status BarelySequence_SetLooping(IntPtr api, Int64 sequenceId,
                                                           bool isLooping);

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformerNote")]
    private static extern Status AddPerformerNoteNative(IntPtr api, Int64 performerId,
                                                        double notePosition, double noteDuration,
                                                        float notePitch, float noteIntensity,
                                                        IntPtr outNoteId);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_GetPosition")]
    private static extern Status BarelyTransport_GetPosition(IntPtr api, IntPtr positionPtr);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_GetTempo")]
    private static extern Status BarelyTransport_GetTempo(IntPtr api, IntPtr tempoPtr);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_IsPlaying")]
    private static extern Status BarelyTransport_IsPlaying(IntPtr api, IntPtr isPlayingPtr);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_Pause")]
    private static extern Status BarelyTransport_Pause(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_Play")]
    private static extern Status BarelyTransport_Play(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_SetPosition")]
    private static extern Status BarelyTransport_SetPosition(IntPtr api, double position);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_SetTempo")]
    private static extern Status BarelyTransport_SetTempo(IntPtr api, double tempo);

    [DllImport(pluginName, EntryPoint = "BarelyTransport_Stop")]
    private static extern Status BarelyTransport_Stop(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOffCallback")]
    private static extern Status SetInstrumentNoteOffCallbackNative(IntPtr api,
                                                                    IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOnCallback")]
    private static extern Status SetInstrumentNoteOnCallbackNative(IntPtr api,
                                                                   IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackBeatCallback")]
    private static extern Status SetPlaybackBeatCallbackNative(IntPtr api,
                                                               IntPtr playbackBeatCallbackPtr);
  }
}
