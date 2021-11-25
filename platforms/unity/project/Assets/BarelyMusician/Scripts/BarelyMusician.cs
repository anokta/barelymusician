using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace BarelyApi {
  /// Class that communicates with the native BarelyMusician API.
  public static class BarelyMusician {
    /// Invalid id.
    public const Int64 InvalidId = -1;

    /// Okay status code.
    public const Int32 OkStatus = 0;

    /// Instrument note off event.
    public delegate void InstrumentNoteOffEvent(Instrument instrument, float notePitch);
    public static event InstrumentNoteOffEvent OnInstrumentNoteOff;

    /// Instrument note on event.
    public delegate void InstrumentNoteOnEvent(Instrument instrument, float notePitch, float noteIntensity);
    public static event InstrumentNoteOnEvent OnInstrumentNoteOn;

    /// Playback beat event.
    public delegate void PlaybackBeatEvent(double position);
    public static event PlaybackBeatEvent OnPlaybackBeat;

    /// Adds new instrument.
    ///
    /// @param instrument Instrument to add.
    /// @return Instrument id.
    public static Int64 AddInstrument(Instrument instrument) {
      Int64 id = InvalidId;
      Type instrumentType = instrument.GetType();
      if (instrumentType == typeof(SynthInstrument)) {
        id = AddSynthInstrumentNative(Handle);
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      if (id != InvalidId) {
        _instruments.Add(id, instrument);
      }
      return id;
    }

    /// Adds new performer.
    ///
    /// @param performer Performer to add.
    /// @return Performer id.
    public static Int64 AddPerformer(Performer performer) {
      return AddPerformerNative(Handle);
    }

    /// Returns the playback position.
    ///
    /// @return Playback position in beats.
    public static double GetPlaybackPosition() {
      return GetPlaybackPositionNative(Handle);
    }

    /// Returns the playback tempo.
    ///
    /// @return Playback tempo.
    public static double GetPlaybackTempo() {
      return GetPlaybackTempoNative(Handle);
    }

    /// Returns whether the playback is currently active or not.
    ///
    /// @return True if playing, false otherwise.
    public static bool IsPlaying() {
      return IsPlayingNative(Handle);
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

    /// Processes the next instrument buffer.
    ///
    /// @param instrument Instrument to process.
    /// @param output Output buffer.
    /// @param numChannels Number of channels.
    public static void ProcessInstrument(Instrument instrument, float[] output, int numChannels) {
      ProcessInstrumentNative(Handle, instrument.Id, AudioSettings.dspTime, output, numChannels,
                              output.Length / numChannels);
    }

    /// Sets all instrument notes off.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentNotesOff(Instrument instrument) {
      return SetAllInstrumentNotesOffNative(Handle, instrument.Id) == OkStatus;
    }

    /// Sets all instrument parameters to default value.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool SetAllInstrumentParamsToDefault(Instrument instrument) {
      return SetAllInstrumentParamsToDefaultNative(Handle, instrument.Id) == OkStatus;
    }

    /// Sets instrument note off.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOff(Instrument instrument, float notePitch) {
      return SetInstrumentNoteOffNative(Handle, instrument.Id, notePitch) == OkStatus;
    }

    /// Sets instrument note on.
    ///
    /// @param instrument Instrument to set.
    /// @param notePitch Note pitch.
    /// @param noteIntensity Note intensity.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentNoteOn(Instrument instrument, float notePitch, float noteIntensity) {
      return SetInstrumentNoteOnNative(Handle, instrument.Id, notePitch, noteIntensity) == OkStatus;
    }

    /// Sets instrument parameter value.
    ///
    /// @param instrument Instrument to set.
    /// @param paramId Parameter id.
    /// @param paramValue Parameter value.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParam(Instrument instrument, int paramId, float paramValue) {
      return SetInstrumentParamNative(Handle, instrument.Id, paramId, paramValue) == OkStatus;
    }

    /// Sets instrument parameter value to default.
    ///
    /// @param instrument Instrument to set.
    /// @param paramId Parameter id.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParamToDefault(Instrument instrument, int paramId) {
      return SetInstrumentParamToDefaultNative(Handle, instrument.Id, paramId) == OkStatus;
    }

    /// Sets playback position.
    ///
    /// @param position Playback position in beats.
    public static void SetPlaybackPosition(double position) {
      SetPlaybackPositionNative(Handle, position);
    }

    /// Sets playback tempo.
    ///
    /// @param tempo Playback tempo in BPM.
    public static void SetPlaybackTempo(double tempo) {
      SetPlaybackTempoNative(Handle, tempo);
    }

    /// Starts the playback.
    public static void StartPlayback() {
      StartPlaybackNative(Handle);
    }

    /// Stops the playback.
    public static void StopPlayback() {
      StopPlaybackNative(Handle);
    }

    /// Updates performer.
    ///
    /// @param performer Performer to update.
    public static void UpdatePerformer(Performer performer) {
      /// TODO(#85): This is a POC implementation only, also missing begin/end position setters.
      SetPerformerBeginOffsetNative(Handle, performer.Id, performer.BeginOffset);
      SetPerformerLoopBeginOffsetNative(Handle, performer.Id, performer.LoopBeginOffset);
      SetPerformerLoopLengthNative(Handle, performer.Id, performer.LoopLength);
      SetPerformerLooping(Handle, performer.Id, performer.IsLooping);
      RemoveAllPerformerNotesNative(Handle, performer.Id);
      foreach (var performerNote in performer.Notes) {
        float pitch = (float)(performer.RootNote + performerNote.note.Pitch - 69) / 12.0f;
        AddPerformerNoteNative(Handle, performer.Id, performerNote.position, performerNote.note.Duration, pitch,
                               performerNote.note.Intensity);
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
          var instance = new GameObject() {
            hideFlags = HideFlags.HideAndDontSave
          }.AddComponent<BarelyMusicianInternal>();
          GameObject.DontDestroyOnLoad(instance.gameObject);
          if (_handle == IntPtr.Zero) {
            Debug.LogError("Could not initialize BarelyMusician.");
            GameObject.DestroyImmediate(instance.gameObject);
          }
        }
        return _handle;
      }
    }
    private static IntPtr _handle = IntPtr.Zero;

    // List of instruments.
    private static Dictionary<Int64, Instrument> _instruments = new Dictionary<Int64, Instrument>();

    // Denotes if the system is shutting down to avoid re-initialization.
    private static bool _isShuttingDown = false;

    // Internal component to update the native state.
    private class BarelyMusicianInternal : MonoBehaviour {
      // Log severity.
      private enum LogSeverity {
        Info = 0,
        Warning = 1,
        Error = 2,
        Fatal = 3,
      }

      // Debug callback.
      private delegate void DebugCallback(int severity, string message);
      private DebugCallback _debugCallback = null;

      // Instrument note off callback.
      private delegate void InstrumentNoteOffCallback(Int64 instrumentId, float notePitch);
      private InstrumentNoteOffCallback _instrumentNoteOffCallback = null;

      // Instrument note on callback.
      private delegate void InstrumentNoteOnCallback(Int64 instrument_id, float notePitch, float noteIntensity);
      private InstrumentNoteOnCallback _instrumentNoteOnCallback = null;

      // Playback beat callback.
      private delegate void PlaybackBeatCallback(double position);
      private PlaybackBeatCallback _playbackBeatCallback = null;

      private void Awake() {
        AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
        _debugCallback = delegate (int severity, string message) {
          message = "{::" + pluginName + "::}" + message;
          switch ((LogSeverity)severity) {
            case LogSeverity.Info:
              Debug.Log(message);
              break;
            case LogSeverity.Warning:
              Debug.LogWarning(message);
              break;
            case LogSeverity.Error:
              Debug.LogError(message);
              break;
            case LogSeverity.Fatal:
              Debug.LogAssertion(message);
              break;
          }
        };
        _handle =
            CreateNative(AudioSettings.outputSampleRate, Marshal.GetFunctionPointerForDelegate(_debugCallback));
        _instrumentNoteOffCallback = delegate (Int64 instrumentId, float notePitch) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(instrumentId, out instrument)) {
            OnInstrumentNoteOff?.Invoke(instrument, notePitch);
          } else {
            Debug.LogWarning("Instrument does not exist: " + instrumentId);
          }
        };
        SetInstrumentNoteOffCallbackNative(_handle, Marshal.GetFunctionPointerForDelegate(_instrumentNoteOffCallback));
        _instrumentNoteOnCallback = delegate (Int64 instrumentId, float notePitch, float noteIntensity) {
          Instrument instrument = null;
          if (_instruments.TryGetValue(instrumentId, out instrument)) {
            OnInstrumentNoteOn?.Invoke(instrument, notePitch, noteIntensity);
          } else {
            Debug.LogWarning("Instrument does not exist: " + instrumentId);
          }
        };
        SetInstrumentNoteOnCallbackNative(_handle, Marshal.GetFunctionPointerForDelegate(_instrumentNoteOnCallback));
        _playbackBeatCallback = delegate (double position) {
          OnPlaybackBeat?.Invoke(position);
        };
        SetPlaybackBeatCallbackNative(_handle, Marshal.GetFunctionPointerForDelegate(_playbackBeatCallback));
      }

      private void OnDestroy() {
        AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
        DestroyNative(_handle);
        _handle = IntPtr.Zero;
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

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformer")]
    private static extern Int64 AddPerformerNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformerInstrument")]
    private static extern Int64 AddPerformerInstrumentNative(IntPtr handle, Int64 performerId, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyAddPerformerNote")]
    private static extern Int64 AddPerformerNoteNative(IntPtr handle, Int64 performerId, double notePosition,
                                                       double noteDuration, float notePitch, float noteIntensity);

    [DllImport(pluginName, EntryPoint = "BarelyAddSynthInstrument")]
    private static extern Int64 AddSynthInstrumentNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyCreateUnity")]
    private static extern IntPtr CreateNative(Int32 sampleRate, IntPtr debugCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelyDestroyUnity")]
    private static extern Int32 DestroyNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyGetPlaybackPosition")]
    private static extern double GetPlaybackPositionNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyGetPlaybackTempo")]
    private static extern double GetPlaybackTempoNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyIsPlaying")]
    private static extern bool IsPlayingNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyProcessInstrument")]
    private static extern Int32 ProcessInstrumentNative(IntPtr handle, Int64 instrumentId, double timestamp,
                                                        [In, Out] float[] output, Int32 numChannels, Int32 numFrames);

    [DllImport(pluginName, EntryPoint = "BarelyRemoveAllPerformerNotes")]
    private static extern Int32 RemoveAllPerformerNotesNative(IntPtr handle, Int64 performerId);

    [DllImport(pluginName, EntryPoint = "BarelyRemoveInstrument")]
    private static extern Int32 RemoveInstrumentNative(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyRemovePerformer")]
    private static extern Int32 RemovePerformerNative(IntPtr handle, Int64 performerId);

    [DllImport(pluginName, EntryPoint = "BarelyRemovePerformerNote")]
    private static extern Int32 RemovePerformerNoteNative(IntPtr handle, Int64 performerId, Int64 noteId);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentNotesOff")]
    private static extern Int32 SetAllInstrumentNotesOffNative(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetAllInstrumentParamsToDefault")]
    private static extern Int32 SetAllInstrumentParamsToDefaultNative(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOffCallback")]
    private static extern Int32 SetInstrumentNoteOffCallbackNative(IntPtr handle, IntPtr noteOffCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOff")]
    private static extern Int32 SetInstrumentNoteOffNative(IntPtr handle, Int64 instrumentId, float notePitch);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOnCallback")]
    private static extern Int32 SetInstrumentNoteOnCallbackNative(IntPtr handle, IntPtr noteOnCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentNoteOn")]
    private static extern Int32 SetInstrumentNoteOnNative(IntPtr handle, Int64 instrumentId, float notePitch,
                                                          float noteIntensity);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParam")]
    private static extern Int32 SetInstrumentParamNative(IntPtr handle, Int64 instrumentId, Int32 paramId,
                                                         float paramValue);

    [DllImport(pluginName, EntryPoint = "BarelySetInstrumentParamToDefault")]
    private static extern Int32 SetInstrumentParamToDefaultNative(IntPtr handle, Int64 instrumentId, Int32 paramId);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerBeginOffset")]
    private static extern Int32 SetPerformerBeginOffsetNative(IntPtr handle, Int64 performerId, double beginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerBeginPosition")]
    private static extern Int32 SetPerformerBeginPositionNative(IntPtr handle, Int64 performerId, IntPtr beginPosition);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerEndPosition")]
    private static extern Int32 SetPerformerEndPositionNative(IntPtr handle, Int64 performerId, IntPtr endPosition);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoopBeginOffset")]
    private static extern Int32 SetPerformerLoopBeginOffsetNative(IntPtr handle, Int64 performerId,
                                                                  double loopBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLoopLength")]
    private static extern Int32 SetPerformerLoopLengthNative(IntPtr handle, Int64 performerId, double loopLength);

    [DllImport(pluginName, EntryPoint = "BarelySetPerformerLooping")]
    private static extern Int32 SetPerformerLooping(IntPtr handle, Int64 performerId, bool looping);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackBeatCallback")]
    private static extern Int32 SetPlaybackBeatCallbackNative(IntPtr handle, IntPtr playbackBeatCallbackPtr);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackPosition")]
    private static extern Int32 SetPlaybackPositionNative(IntPtr handle, double position);

    [DllImport(pluginName, EntryPoint = "BarelySetPlaybackTempo")]
    private static extern Int32 SetPlaybackTempoNative(IntPtr handle, double tempo);

    [DllImport(pluginName, EntryPoint = "BarelySetSampleRate")]
    private static extern Int32 SetSampleRateNative(IntPtr handle, Int32 sampleRate);

    [DllImport(pluginName, EntryPoint = "BarelyStartPlayback")]
    private static extern Int32 StartPlaybackNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyStopPlayback")]
    private static extern Int32 StopPlaybackNative(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyUpdate")]
    private static extern Int32 UpdateNative(IntPtr handle, double timestamp);
  }
}
