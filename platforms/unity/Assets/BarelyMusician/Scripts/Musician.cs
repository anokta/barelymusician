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

    /// Beat event.
    public delegate void BeatEvent(double position);
    public static event BeatEvent OnBeat;

    /// Note duration definition.
    [StructLayout(LayoutKind.Sequential)]
    public struct NoteDurationDefinition {
      /// Value.
      public double duration;
    }

    /// Note intensity definition.
    [StructLayout(LayoutKind.Sequential)]
    public struct NoteIntensityDefinition {
      /// Value.
      public float intensity;
    }

    /// Note pitch types.
    public enum NotePitchType {
      /// Absolute pitch.
      AbsolutePitch = 0,
      /// Relative pitch with respect to root note.
      RelativePitch = 1,
      /// Scale index with respect to root note and scale.
      ScaleIndex = 2,
    }

    /// Note pitch definition.
    [StructLayout(LayoutKind.Explicit)]
    public struct NotePitchDefinition {
      /// Type.
      [FieldOffset(0)]
      public NotePitchType type;

      /// Absolute pitch.
      [FieldOffset(4)]
      public float absolutePitch;
      /// Relative pitch.
      [FieldOffset(4)]
      public float relativePitch;
      /// Scale index.
      [FieldOffset(4)]
      public float scaleIndex;
    }

    /// Note definition.
    [StructLayout(LayoutKind.Sequential)]
    public struct NoteDefinition {
      /// Duration definition.
      public NoteDurationDefinition durationDefinition;

      /// Denotes whether duration adjustment should be bypassed or not.
      public bool bypassDurationAdjustment;

      /// Intensity definition.
      public NoteIntensityDefinition intensityDefinition;

      /// Denotes whether intensity adjustment should be bypassed or not.
      public bool bypassIntensityAdjustment;

      /// Pitch definition.
      public NotePitchDefinition pitchDefinition;

      /// Denotes whether pitch adjustment should be bypassed or not.
      public bool bypassPitchAdjustment;
    }

    /// Adds new instrument.
    ///
    /// @param instrument Instrument to add.
    /// @return Instrument id.
    public static Int64 AddInstrument(Instrument instrument) {
      Int64 instrumentId = InvalidId;
      Type instrumentType = instrument.GetType();
      if (instrumentType == typeof(SynthInstrument)) {
        Status status =
            BarelyExamples_CreateSynthInstrument(Api, AudioSettings.outputSampleRate, _int64Ptr);
        if (IsOk(status)) {
          instrumentId = Marshal.ReadInt64(_int64Ptr);
        } else {
          Debug.LogError("Failed to add instrument (" + instrument.name + "): " + status);
        }
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      if (instrumentId != InvalidId) {
        BarelyInstrument_SetNoteOffCallback(
            _api, instrumentId, Marshal.GetFunctionPointerForDelegate(instrument._noteOffCallback),
            IntPtr.Zero);
        BarelyInstrument_SetNoteOnCallback(
            _api, instrumentId, Marshal.GetFunctionPointerForDelegate(instrument._noteOnCallback),
            IntPtr.Zero);
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
      Status status = BarelyMusician_GetPosition(Api, _doublePtr);
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
      Status status = BarelyMusician_GetTempo(Api, _doublePtr);
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
      Status status = BarelyMusician_IsPlaying(Api, _booleanPtr);
      if (IsOk(status)) {
        return Marshal.PtrToStructure<Boolean>(_booleanPtr);
      } else {
        Debug.LogError("Failed to get playback state: " + status);
      }
      return false;
    }

    /// Pauses playback.
    ///
    /// @return True if success, false otherwise.
    public static bool Pause() {
      return IsOk(BarelyMusician_Stop(Api));
    }

    /// Starts playback.
    ///
    /// @return True if success, false otherwise.
    public static bool Play() {
      return IsOk(BarelyMusician_Start(Api));
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
    }

    /// Removes sequence.
    ///
    /// @param sequence Sequence to remove.
    public static void RemoveSequence(Sequence sequence) {
      BarelySequence_Destroy(Api, sequence.Id);
    }

    /// Resets all instrument parameters to default value.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool ResetAllInstrumentParameters(Instrument instrument) {
      return IsOk(BarelyInstrument_ResetAllParameters(Api, instrument.Id));
    }

    /// Resets instrument parameter to default value.
    ///
    /// @param instrument Instrument to set.
    /// @param index Parameter index.
    /// @return True if success, false otherwise.
    public static bool ResetInstrumentParameter(Instrument instrument, int index) {
      return IsOk(BarelyInstrument_ResetParameter(Api, instrument.Id, index));
    }

    /// Sets instrument parameter value.
    ///
    /// @param instrument Instrument to set.
    /// @param index Parameter index.
    /// @param value Parameter value.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParameter(Instrument instrument, int index, double value) {
      return IsOk(BarelyInstrument_SetParameter(Api, instrument.Id, index, value));
    }

    /// Sets playback position.
    ///
    /// @param position Playback position in beats.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackPosition(double position) {
      return IsOk(BarelyMusician_SetPosition(Api, position));
    }

    /// Sets playback tempo.
    ///
    /// @param tempo Playback tempo in bpm.
    /// @return True if success, false otherwise.
    public static bool SetPlaybackTempo(double tempo) {
      return IsOk(BarelyMusician_SetTempo(Api, tempo));
    }

    /// Starts instrument note.
    ///
    /// @param instrument Instrument to set.
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    /// @return True if success, false otherwise.
    public static bool StartInstrumentNote(Instrument instrument, float pitch, float intensity) {
      return IsOk(BarelyInstrument_StartNote(Api, instrument.Id, pitch, intensity));
    }

    /// Stops playback.
    ///
    /// @return True if success, false otherwise.
    public static bool Stop() {
      return Pause() && SetPlaybackPosition(0.0);
    }

    /// Stops all instrument notes.
    ///
    /// @param instrument Instrument to set.
    /// @return True if success, false otherwise.
    public static bool StopAllInstrumentNotes(Instrument instrument) {
      return IsOk(BarelyInstrument_StopAllNotes(Api, instrument.Id));
    }

    /// Stops instrument note.
    ///
    /// @param instrument Instrument to set.
    /// @param pitch Note pitch.
    /// @return True if success, false otherwise.
    public static bool StopInstrumentNote(Instrument instrument, float pitch) {
      return IsOk(BarelyInstrument_StopNote(Api, instrument.Id, pitch));
    }

    /// Updates sequence.
    ///
    /// @param sequence Sequence to update.
    /// TODO(#85): This is a POC implementation only.
    public static void UpdateSequence(Sequence sequence, bool changed) {
      BarelySequence_SetBeginOffset(Api, sequence.Id, sequence.BeginOffset);
      BarelySequence_SetBeginPosition(Api, sequence.Id, sequence.BeginPosition);
      BarelySequence_SetEndPosition(Api, sequence.Id, sequence.EndPosition);
      BarelySequence_SetLooping(Api, sequence.Id, sequence.Loop);
      BarelySequence_SetLoopBeginOffset(Api, sequence.Id, sequence.LoopBeginOffset);
      BarelySequence_SetLoopLength(Api, sequence.Id, sequence.LoopLength);
      BarelySequence_SetInstrument(Api, sequence.Id,
                                   sequence.Instrument ? sequence.Instrument.Id : InvalidId);

      if (changed) {
        BarelySequence_RemoveAllNotes(Api, sequence.Id);
        NoteDefinition definition = new NoteDefinition {};
        definition.bypassDurationAdjustment = false;
        definition.bypassIntensityAdjustment = false;
        definition.bypassPitchAdjustment = false;
        definition.pitchDefinition.type = NotePitchType.AbsolutePitch;
        foreach (var sequenceNote in sequence.Notes) {
          definition.durationDefinition.duration = sequenceNote.note.Duration;
          definition.pitchDefinition.absolutePitch =
              (float)(sequence.RootNote + sequenceNote.note.Pitch - 69) / 12.0f;
          definition.intensityDefinition.intensity = sequenceNote.note.Intensity;
          BarelySequence_AddNote(Api, sequence.Id, sequenceNote.position, definition, _int64Ptr);
        }
      }
    }

    // Singleton api.
    private static IntPtr Api {
      get {
        if (_isShuttingDown) {
          return IntPtr.Zero;
        }
        if (_api == IntPtr.Zero) {
          if (_intPtrPtr == IntPtr.Zero) {
            _intPtrPtr = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
          }
          var musician = new GameObject() { hideFlags = HideFlags.HideAndDontSave }
                             .AddComponent<MusicianInternal>();
          GameObject.DontDestroyOnLoad(musician.gameObject);
          if (_api != IntPtr.Zero) {
            _booleanPtr = Marshal.AllocHGlobal(Marshal.SizeOf<Boolean>());
            _doublePtr = Marshal.AllocHGlobal(Marshal.SizeOf<Double>());
            _int64Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int64>());
          } else {
            Debug.LogError("Could not initialize BarelyMusician.");
            GameObject.DestroyImmediate(musician.gameObject);
          }
          if (_intPtrPtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_intPtrPtr);
            _intPtrPtr = IntPtr.Zero;
          }
        }
        return _api;
      }
    }
    private static IntPtr _api = IntPtr.Zero;

    // `Boolean` type pointer.
    private static IntPtr _booleanPtr = IntPtr.Zero;

    // `Double` type pointer.
    private static IntPtr _doublePtr = IntPtr.Zero;

    // `Int64` type pointer.
    private static IntPtr _int64Ptr = IntPtr.Zero;

    // `IntPtr` type pointer.
    private static IntPtr _intPtrPtr = IntPtr.Zero;

    // Denotes if the system is shutting down to avoid re-initialization.
    private static bool _isShuttingDown = false;

    // Internal component to manage the native state.
    private class MusicianInternal : MonoBehaviour {
      // Beat callback.
      private delegate void BeatCallback(double position, double timestamp);
      private BeatCallback _beatCallback = null;

      // Latency in seconds.
      private double _latency = 0.0;

      private void Awake() {
        AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
        if (!IsOk(BarelyMusician_Create(_intPtrPtr))) {
          return;
        }
        _api = Marshal.PtrToStructure<IntPtr>(_intPtrPtr);
        _beatCallback = delegate(double position, double timestamp) {
          OnBeat?.Invoke(position);
        };
        BarelyMusician_SetBeatCallback(_api, Marshal.GetFunctionPointerForDelegate(_beatCallback),
                                       IntPtr.Zero);
        var config = AudioSettings.GetConfiguration();
        _latency = (double)(config.dspBufferSize) / (double)config.sampleRate;
      }

      private void OnDestroy() {
        AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
        BarelyMusician_Destroy(_api);
        _api = IntPtr.Zero;
        Marshal.FreeHGlobal(_booleanPtr);
        Marshal.FreeHGlobal(_doublePtr);
        Marshal.FreeHGlobal(_int64Ptr);
        Marshal.FreeHGlobal(_intPtrPtr);
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        BarelyMusician_Destroy(_api);
        _api = IntPtr.Zero;
      }

      private void OnAudioConfigurationChanged(bool deviceWasChanged) {
        var config = AudioSettings.GetConfiguration();
        _latency = (double)(config.dspBufferSize) / (double)config.sampleRate;
        foreach (var instrument in FindObjectsOfType<Instrument>()) {
          instrument.enabled = false;
          instrument.enabled = true;
        }
      }

      private void LateUpdate() {
        double lookahead = System.Math.Max(_latency, (double)Time.smoothDeltaTime);
        UpdateNative(_api, AudioSettings.dspTime + lookahead);
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

    // Returns whether given `status` is ok.
    private static bool IsOk(Status status) {
      return (status == Status.Ok);
    }

#if !UNITY_EDITOR && UNITY_IOS
    private const string pluginName = "__Internal";
#else
    private const string pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

    [DllImport(pluginName, EntryPoint = "BarelyExamples_CreateSynthInstrument")]
    private static extern Status BarelyExamples_CreateSynthInstrument(IntPtr api, Int32 sampleRate,
                                                                      IntPtr outInstrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
    private static extern Status BarelyInstrument_Destroy(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
    private static extern Status BarelyInstrument_Process(IntPtr api, Int64 instrumentId,
                                                          double timestamp,
                                                          [In, Out] float[] output,
                                                          Int32 numOutputChannels,
                                                          Int32 numOutputFrames);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllParameters")]
    private static extern Status BarelyInstrument_ResetAllParameters(IntPtr api,
                                                                     Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetParameter")]
    private static extern Status BarelyInstrument_ResetParameter(IntPtr api, Int64 instrumentId,
                                                                 Int32 index);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffCallback")]
    private static extern Status BarelyInstrument_SetNoteOffCallback(IntPtr api, Int64 instrumentId,
                                                                     IntPtr noteOffCallback,
                                                                     IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnCallback")]
    private static extern Status BarelyInstrument_SetNoteOnCallback(IntPtr api, Int64 instrumentId,
                                                                    IntPtr noteOnCallback,
                                                                    IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetParameter")]
    private static extern Status BarelyInstrument_SetParameter(IntPtr api, Int64 instrumentId,
                                                               Int32 index, double value);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StartNote")]
    private static extern Status BarelyInstrument_StartNote(IntPtr api, Int64 instrumentId,
                                                            float pitch, float intensity);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopAllNotes")]
    private static extern Status BarelyInstrument_StopAllNotes(IntPtr api, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopNote")]
    private static extern Status BarelyInstrument_StopNote(IntPtr api, Int64 instrumentId,
                                                           float pitch);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
    private static extern Status BarelyMusician_Create(IntPtr outApi);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
    private static extern Status BarelyMusician_Destroy(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetPosition")]
    private static extern Status BarelyMusician_GetPosition(IntPtr api, IntPtr positionPtr);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
    private static extern Status BarelyMusician_GetTempo(IntPtr api, IntPtr tempoPtr);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_IsPlaying")]
    private static extern Status BarelyMusician_IsPlaying(IntPtr api, IntPtr isPlayingPtr);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetBeatCallback")]
    private static extern Status BarelyMusician_SetBeatCallback(IntPtr api, IntPtr beatCallback,
                                                                IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetPosition")]
    private static extern Status BarelyMusician_SetPosition(IntPtr api, double position);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
    private static extern Status BarelyMusician_SetTempo(IntPtr api, double tempo);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Start")]
    private static extern Status BarelyMusician_Start(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Stop")]
    private static extern Status BarelyMusician_Stop(IntPtr api);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
    private static extern Status UpdateNative(IntPtr api, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelySequence_AddNote")]
    private static extern Status BarelySequence_AddNote(IntPtr api, Int64 sequenceId,
                                                        double position, NoteDefinition definition,
                                                        IntPtr outNoteId);

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
  }
}
