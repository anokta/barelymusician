using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Class that wraps musician.
  public static class Musician {
    /// Invalid identifier.
    public const Int64 InvalidId = -1;

    // Adjust note callback.
    ///
    /// @param definition Reference to mutable note definition.
    public delegate void AdjustNoteCallback(ref Note.Definition definition);
    public static event AdjustNoteCallback OnAdjustNote;

    /// Beat callback.
    ///
    /// @param position Beat position in beats.
    /// @param timestamp Beat timestamp in seconds.
    public delegate void BeatCallback(double position, double timestamp);
    public static event BeatCallback OnBeat;

    /// Adds new instrument.
    ///
    /// @param instrument Instrument.
    /// @param noteOffCallback Reference to note off callback.
    /// @param noteOnCallback Reference to note on callback.
    /// @return Instrument identifier.
    public static Int64 AddInstrument(Instrument instrument,
                                      ref Instrument.NoteOffCallback noteOffCallback,
                                      ref Instrument.NoteOnCallback noteOnCallback) {
      Int64 instrumentId = InvalidId;
      Type instrumentType = instrument.GetType();
      if (instrumentType == typeof(SynthInstrument)) {
        Status status = BarelyInstrument_CreateOfType(Handle, InstrumentType.Synth,
                                                      AudioSettings.outputSampleRate, _int64Ptr);
        if (IsOk(status)) {
          instrumentId = Marshal.ReadInt64(_int64Ptr);
        } else {
          Debug.LogError("Failed to add instrument '" + instrument.name + "': " + status);
        }
      } else {
        Debug.LogError("Unsupported instrument type: " + instrumentType);
      }
      if (instrumentId != InvalidId) {
        BarelyInstrument_SetNoteOffCallback(_handle, instrumentId,
                                            Marshal.GetFunctionPointerForDelegate(noteOffCallback),
                                            IntPtr.Zero);
        BarelyInstrument_SetNoteOnCallback(_handle, instrumentId,
                                           Marshal.GetFunctionPointerForDelegate(noteOnCallback),
                                           IntPtr.Zero);
      }
      return instrumentId;
    }

    /// Adds new sequence note.
    ///
    /// @param sequence Sequence.
    /// @param definition Note definition.
    /// @param position Note position.
    /// @return Note identifier.
    public static Int64 AddNote(Sequence sequence, Note.Definition definition, double position) {
      Int64 noteId = InvalidId;
      Status status = BarelyNote_Create(Handle, sequence.Id, definition, position, _int64Ptr);
      if (IsOk(status)) {
        noteId = Marshal.ReadInt64(_int64Ptr);
      } else {
        Debug.LogError("Failed to add sequence note to '" + sequence.name + "': " + status);
      }
      return noteId;
    }

    /// Adds new sequence.
    ///
    /// @param sequence Sequence.
    /// @return Sequence identifier.
    public static Int64 AddSequence(Sequence sequence) {
      Int64 sequenceId = InvalidId;
      Status status = BarelySequence_Create(Handle, _int64Ptr);
      if (IsOk(status)) {
        sequenceId = Marshal.ReadInt64(_int64Ptr);
      } else {
        Debug.LogError("Failed to add sequence '" + sequence.name + "': " + status);
      }
      return sequenceId;
    }

    /// Returns the playback position.
    ///
    /// @return Playback position in beats.
    public static double GetPlaybackPosition() {
      Status status = BarelyMusician_GetPosition(Handle, _doublePtr);
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
      Status status = BarelyMusician_GetTempo(Handle, _doublePtr);
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
      Status status = BarelyMusician_IsPlaying(Handle, _booleanPtr);
      if (IsOk(status)) {
        return Marshal.PtrToStructure<Boolean>(_booleanPtr);
      } else {
        Debug.LogError("Failed to get playback state: " + status);
      }
      return false;
    }

    /// Pauses playback.
    public static void Pause() {
      BarelyMusician_Stop(Handle);
    }

    /// Starts playback.
    public static void Play() {
      BarelyMusician_Start(Handle);
    }

    /// Processes the next instrument buffer.
    ///
    /// @param instrument Instrument.
    /// @param output Output buffer.
    /// @param numOutputChannels Number of output channels.
    public static void ProcessInstrument(Instrument instrument, float[] output,
                                         int numOutputChannels) {
      if (IsOk(BarelyInstrument_Process(Handle, instrument.Id, _output, numOutputChannels,
                                        output.Length / numOutputChannels,
                                        AudioSettings.dspTime))) {
        for (int i = 0; i < output.Length; ++i) {
          output[i] = (float)_output[i];
        }
      } else {
        for (int i = 0; i < output.Length; ++i) {
          output[i] = 0;
        }
      }
    }

    /// Removes instrument.
    ///
    /// @param instrument Instrument.
    public static void RemoveInstrument(Instrument instrument) {
      BarelyInstrument_Destroy(Handle, instrument.Id);
    }

    /// Removes note.
    ///
    /// @param sequence Sequence.
    /// @param note Note.
    public static void RemoveNote(Sequence sequence, Note note) {
      BarelyNote_Destroy(Handle, sequence.Id, note.Id);
    }

    /// Removes sequence.
    ///
    /// @param sequence Sequence.
    public static void RemoveSequence(Sequence sequence) {
      BarelySequence_Destroy(Handle, sequence.Id);
    }

    /// Resets all instrument parameters to default value.
    ///
    /// @param instrument Instrument.
    public static void ResetAllInstrumentParameters(Instrument instrument) {
      BarelyInstrument_ResetAllParameters(Handle, instrument.Id);
    }

    /// Resets instrument parameter to default value.
    ///
    /// @param instrument Instrument.
    /// @param index Parameter index.
    /// @return True if success, false otherwise.
    public static bool ResetInstrumentParameter(Instrument instrument, int index) {
      return BarelyInstrument_ResetParameter(Handle, instrument.Id, index) !=
             Status.InvalidArgument;
    }

    /// Sets instrument parameter value.
    ///
    /// @param instrument Instrument.
    /// @param index Parameter index.
    /// @param value Parameter value.
    /// @return True if success, false otherwise.
    public static bool SetInstrumentParameter(Instrument instrument, int index, double value) {
      return BarelyInstrument_SetParameter(Handle, instrument.Id, index, value) !=
             Status.InvalidArgument;
    }

    /// Sets note definition.
    ///
    /// @param sequence Sequence.
    /// @param note Note.
    /// @param definition Definition.
    public static void SetNoteDefinition(Sequence sequence, Note note, Note.Definition definition) {
      BarelyNote_SetDefinition(Handle, sequence.Id, note.Id, definition);
    }

    /// Sets note position.
    ///
    /// @param sequence Sequence.
    /// @param note Note.
    /// @param position Position.
    public static void SetNotePosition(Sequence sequence, Note note, double position) {
      BarelyNote_SetPosition(Handle, sequence.Id, note.Id, position);
    }

    /// Sets playback position.
    ///
    /// @param position Playback position in beats.
    public static void SetPlaybackPosition(double position) {
      BarelyMusician_SetPosition(Handle, position);
    }

    /// Sets playback tempo.
    ///
    /// @param tempo Playback tempo in bpm.
    public static void SetPlaybackTempo(double tempo) {
      BarelyMusician_SetTempo(Handle, tempo);
    }

    /// Starts instrument note.
    ///
    /// @param instrument Instrument.
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public static void StartInstrumentNote(Instrument instrument, double pitch, double intensity) {
      BarelyInstrument_StartNote(Handle, instrument.Id, pitch, intensity);
    }

    /// Stops playback.
    public static void Stop() {
      Pause();
      SetPlaybackPosition(0.0);
    }

    /// Stops all instrument notes.
    ///
    /// @param instrument Instrument.
    /// @return True if success, false otherwise.
    public static void StopAllInstrumentNotes(Instrument instrument) {
      BarelyInstrument_StopAllNotes(Handle, instrument.Id);
    }

    /// Stops instrument note.
    ///
    /// @param instrument Instrument.
    /// @param pitch Note pitch.
    /// @return True if success, false otherwise.
    public static void StopInstrumentNote(Instrument instrument, double pitch) {
      BarelyInstrument_StopNote(Handle, instrument.Id, pitch);
    }

    /// Updates sequence.
    ///
    /// @param sequence Sequence.
    /// TODO(#85): This is a POC implementation only.
    public static void UpdateSequence(Sequence sequence, bool changed) {
      BarelySequence_SetBeginOffset(Handle, sequence.Id, sequence.BeginOffset);
      BarelySequence_SetBeginPosition(Handle, sequence.Id, sequence.BeginPosition);
      BarelySequence_SetEndPosition(Handle, sequence.Id, sequence.EndPosition);
      BarelySequence_SetLooping(Handle, sequence.Id, sequence.Loop);
      BarelySequence_SetLoopBeginOffset(Handle, sequence.Id, sequence.LoopBeginOffset);
      BarelySequence_SetLoopLength(Handle, sequence.Id, sequence.LoopLength);
      BarelySequence_SetInstrument(Handle, sequence.Id,
                                   sequence.Instrument ? sequence.Instrument.Id : InvalidId);

      if (changed) {
        sequence.NativeNotes = new Note[sequence.Notes.Length];
        Note.Definition definition = new Note.Definition {};
        definition.pitch.type = Note.PitchType.AbsolutePitch;
        for (int i = 0; i < sequence.NativeNotes.Length; ++i) {
          definition.duration = sequence.Notes[i].note.Duration;
          definition.pitch.value.absolutePitch =
              (double)(sequence.RootNote + sequence.Notes[i].note.Pitch - 69) / 12.0;
          definition.intensity = sequence.Notes[i].note.Intensity;
          sequence.NativeNotes[i] = new Note(sequence, definition, sequence.Notes[i].position);
        }
      }
    }

    // Singleton musician handle.
    private static IntPtr Handle {
      get {
        if (_isShuttingDown) {
          return IntPtr.Zero;
        }
        if (_handle == IntPtr.Zero) {
          if (_intPtrPtr == IntPtr.Zero) {
            _intPtrPtr = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
          }
          var musician = new GameObject() { hideFlags = HideFlags.HideAndDontSave }
                             .AddComponent<MusicianInternal>();
          GameObject.DontDestroyOnLoad(musician.gameObject);
          if (_handle != IntPtr.Zero) {
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
        return _handle;
      }
    }
    private static IntPtr _handle = IntPtr.Zero;

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

    // Internal output buffer.
    private static double[] _output = null;

    // Internal component to manage the native state.
    private class MusicianInternal : MonoBehaviour {
      // Adjust note callback.
      private AdjustNoteCallback _adjustNoteCallback = null;

      // Beat callback.
      private BeatCallback _beatCallback = null;

      // Latency in seconds.
      private double _latency = 0.0;

      private void Awake() {
        AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
        if (!IsOk(BarelyMusician_Create(_intPtrPtr))) {
          return;
        }
        _handle = Marshal.PtrToStructure<IntPtr>(_intPtrPtr);
        _adjustNoteCallback = delegate(ref Note.Definition definition) {
          OnAdjustNote?.Invoke(ref definition);
        };
        _beatCallback = delegate(double position, double timestamp) {
          OnBeat?.Invoke(position, timestamp);
        };
        BarelyMusician_SetAdjustNoteCallback(
            _handle, Marshal.GetFunctionPointerForDelegate(_adjustNoteCallback), IntPtr.Zero);
        BarelyMusician_SetBeatCallback(
            _handle, Marshal.GetFunctionPointerForDelegate(_beatCallback), IntPtr.Zero);
        var config = AudioSettings.GetConfiguration();
        _latency = (double)(config.dspBufferSize) / (double)config.sampleRate;
        _output = new double[config.dspBufferSize * (int)config.speakerMode];
      }

      private void OnDestroy() {
        AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
        BarelyMusician_Destroy(_handle);
        _handle = IntPtr.Zero;
        Marshal.FreeHGlobal(_booleanPtr);
        Marshal.FreeHGlobal(_doublePtr);
        Marshal.FreeHGlobal(_int64Ptr);
        Marshal.FreeHGlobal(_intPtrPtr);
      }

      private void OnApplicationQuit() {
        _isShuttingDown = true;
        BarelyMusician_Destroy(_handle);
        _handle = IntPtr.Zero;
      }

      private void OnAudioConfigurationChanged(bool deviceWasChanged) {
        var config = AudioSettings.GetConfiguration();
        _latency = (double)(config.dspBufferSize) / (double)config.sampleRate;
        _output = new double[config.dspBufferSize * (int)config.speakerMode];
        BarelyMusician_SetTimestamp(_handle, AudioSettings.dspTime);
        foreach (var instrument in FindObjectsOfType<Instrument>()) {
          instrument.enabled = false;
          instrument.enabled = true;
        }
      }

      private void LateUpdate() {
        double lookahead = System.Math.Max(_latency, (double)Time.smoothDeltaTime);
        BarelyMusician_Update(_handle, AudioSettings.dspTime + lookahead);
      }
    }

    // Instrument type.
    private enum InstrumentType {
      /// Synth instrument.
      Synth = 0,
      /// Percussion instrument.
      Percussion = 1,
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

    // TODO(#105): Add `Instrument.Definition` to support generic `BarelyInstrument_Create`.
    // [DllImport(pluginName, EntryPoint = "BarelyInstrument_Create")]
    // private static extern Status BarelyInstrument_Create(IntPtr handle,
    //                                                      Instrument.Definition definition,
    //                                                      Int32 frameRate, IntPtr
    //                                                      outInstrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_CreateOfType")]
    private static extern Status BarelyInstrument_CreateOfType(IntPtr handle, InstrumentType type,
                                                               Int32 frameRate,
                                                               IntPtr outInstrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_Destroy")]
    private static extern Status BarelyInstrument_Destroy(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetParameter")]
    private static extern Status BarelyInstrument_GetParameter(IntPtr handle, Int64 instrumentId,
                                                               Int32 index, IntPtr outValue);

    // TODO(#105): Add `ParameterDefinition` to support `BarelyInstrument_GetParameterDefinition`.
    // [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetParameterDefinition")]
    // private static extern Status BarelyInstrument_GetParameterDefinition(IntPtr handle,
    //                                                                      Int64 instrumentId,
    //                                                                      Int32 index,
    //                                                                      IntPtr outDefinition);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_IsNoteOn")]
    private static extern Status BarelyInstrument_IsNoteOn(IntPtr handle, Int64 instrumentId,
                                                           double pitch, IntPtr outIsNoteOn);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_Process")]
    private static extern Status BarelyInstrument_Process(IntPtr handle, Int64 instrumentId,
                                                          [In, Out] double[] output,
                                                          Int32 numOutputChannels,
                                                          Int32 numOutputFrames, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllParameters")]
    private static extern Status BarelyInstrument_ResetAllParameters(IntPtr handle,
                                                                     Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetParameter")]
    private static extern Status BarelyInstrument_ResetParameter(IntPtr handle, Int64 instrumentId,
                                                                 Int32 index);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetData")]
    private static extern Status BarelyInstrument_SetData(IntPtr handle, Int64 instrumentId,
                                                          [In] IntPtr data, Int32 size);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOffCallback")]
    private static extern Status BarelyInstrument_SetNoteOffCallback(IntPtr handle,
                                                                     Int64 instrumentId,
                                                                     IntPtr callback,
                                                                     IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetNoteOnCallback")]
    private static extern Status BarelyInstrument_SetNoteOnCallback(IntPtr handle,
                                                                    Int64 instrumentId,
                                                                    IntPtr callback,
                                                                    IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_SetParameter")]
    private static extern Status BarelyInstrument_SetParameter(IntPtr handle, Int64 instrumentId,
                                                               Int32 index, double value);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StartNote")]
    private static extern Status BarelyInstrument_StartNote(IntPtr handle, Int64 instrumentId,
                                                            double pitch, double intensity);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopAllNotes")]
    private static extern Status BarelyInstrument_StopAllNotes(IntPtr handle, Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_StopNote")]
    private static extern Status BarelyInstrument_StopNote(IntPtr handle, Int64 instrumentId,
                                                           double pitch);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Create")]
    private static extern Status BarelyMusician_Create(IntPtr outHandle);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Destroy")]
    private static extern Status BarelyMusician_Destroy(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetNote")]
    private static extern Status BarelyMusician_GetNote(IntPtr handle,
                                                        Note.PitchDefinition definition,
                                                        IntPtr outPitch);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetPosition")]
    private static extern Status BarelyMusician_GetPosition(IntPtr handle, IntPtr outPosition);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetRootNote")]
    private static extern Status BarelyMusician_GetRootNote(IntPtr handle, IntPtr outRootPitch);

    [DllImport(pluginName, EntryPoint = "BarelyInstrument_GetScale")]
    private static extern Status BarelyInstrument_GetScale(IntPtr handle, Int64 instrumentId,
                                                           IntPtr outScalePitches,
                                                           Int32 numScalePitches);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTempo")]
    private static extern Status BarelyMusician_GetTempo(IntPtr handle, IntPtr outTempo);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestamp")]
    private static extern Status BarelyMusician_GetTimestamp(IntPtr handle, IntPtr outTimestamp);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_GetTimestampAtPosition")]
    private static extern Status BarelyMusician_GetTimestampAtPosition(IntPtr handle,
                                                                       double position,
                                                                       IntPtr outTimestamp);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_IsPlaying")]
    private static extern Status BarelyMusician_IsPlaying(IntPtr handle, IntPtr outIsPlaying);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetAdjustNoteCallback")]
    private static extern Status BarelyMusician_SetAdjustNoteCallback(IntPtr handle,
                                                                      IntPtr callback,
                                                                      IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetAdjustParameterAutomationCallback")]
    private static extern Status BarelyMusician_SetAdjustParameterAutomationCallback(
        IntPtr handle, IntPtr callback, IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetBeatCallback")]
    private static extern Status BarelyMusician_SetBeatCallback(IntPtr handle, IntPtr callback,
                                                                IntPtr userData);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetPosition")]
    private static extern Status BarelyMusician_SetPosition(IntPtr handle, double position);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetRootNote")]
    private static extern Status BarelyMusician_SetRootNote(IntPtr handle, double rootPitch);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetScale")]
    private static extern Status BarelyMusician_SetScale(IntPtr handle, [In] double[] scalePitches,
                                                         Int32 numScalePitches);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTempo")]
    private static extern Status BarelyMusician_SetTempo(IntPtr handle, double tempo);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_SetTimestamp")]
    private static extern Status BarelyMusician_SetTimestamp(IntPtr handle, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Start")]
    private static extern Status BarelyMusician_Start(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Stop")]
    private static extern Status BarelyMusician_Stop(IntPtr handle);

    [DllImport(pluginName, EntryPoint = "BarelyMusician_Update")]
    private static extern Status BarelyMusician_Update(IntPtr handle, double timestamp);

    [DllImport(pluginName, EntryPoint = "BarelyNote_Create")]
    private static extern Status BarelyNote_Create(IntPtr handle, Int64 sequenceId,
                                                   Note.Definition definition, double position,
                                                   IntPtr outNoteId);

    [DllImport(pluginName, EntryPoint = "BarelyNote_Destroy")]
    private static extern Status BarelyNote_Destroy(IntPtr handle, Int64 sequenceId, Int64 noteId);

    [DllImport(pluginName, EntryPoint = "BarelyNote_GetDefinition")]
    private static extern Status BarelyNote_GetDefinition(IntPtr handle, Int64 sequenceId,
                                                          Int64 noteId, IntPtr outDefinition);

    [DllImport(pluginName, EntryPoint = "BarelyNote_GetPosition")]
    private static extern Status BarelyNote_GetPosition(IntPtr handle, Int64 sequenceId,
                                                        Int64 noteId, IntPtr outPosition);

    [DllImport(pluginName, EntryPoint = "BarelyNote_SetDefinition")]
    private static extern Status BarelyNote_SetDefinition(IntPtr handle, Int64 sequenceId,
                                                          Int64 noteId, Note.Definition definition);

    [DllImport(pluginName, EntryPoint = "BarelyNote_SetPosition")]
    private static extern Status BarelyNote_SetPosition(IntPtr handle, Int64 sequenceId,
                                                        Int64 noteId, double position);

    // TODO(#98): Add `ParameterAutomation` support.

    [DllImport(pluginName, EntryPoint = "BarelySequence_Create")]
    private static extern Status BarelySequence_Create(IntPtr handle, IntPtr outSequenceId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_Destroy")]
    private static extern Status BarelySequence_Destroy(IntPtr handle, Int64 sequenceId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_GetBeginOffset")]
    private static extern Status BarelySequence_GetBeginOffset(IntPtr handle, Int64 sequenceId,
                                                               IntPtr outBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySequence_GetBeginPosition")]
    private static extern Status BarelySequence_GetBeginPosition(IntPtr handle, Int64 sequenceId,
                                                                 IntPtr outBeginPosition);

    [DllImport(pluginName, EntryPoint = "BarelySequence_GetEndPosition")]
    private static extern Status BarelySequence_GetEndPosition(IntPtr handle, Int64 sequenceId,
                                                               IntPtr outEndPosition);

    [DllImport(pluginName, EntryPoint = "BarelySequence_GetInstrument")]
    private static extern Status BarelySequence_GetInstrument(IntPtr handle, Int64 sequenceId,
                                                              IntPtr outInstrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_GetLoopBeginOffset")]
    private static extern Status BarelySequence_GetLoopBeginOffset(IntPtr handle, Int64 sequenceId,
                                                                   IntPtr outLoopBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySequence_GetLoopLength")]
    private static extern Status BarelySequence_GetLoopLength(IntPtr handle, Int64 sequenceId,
                                                              IntPtr outLoopLength);

    [DllImport(pluginName, EntryPoint = "BarelySequence_IsEmpty")]
    private static extern Status BarelySequence_IsEmpty(IntPtr handle, Int64 sequenceId,
                                                        IntPtr outIsEmpty);

    [DllImport(pluginName, EntryPoint = "BarelySequence_IsLooping")]
    private static extern Status BarelySequence_IsLooping(IntPtr handle, Int64 sequenceId,
                                                          IntPtr outIsLooping);

    [DllImport(pluginName, EntryPoint = "BarelySequence_IsSkippingAdjustments")]
    private static extern Status BarelySequence_IsSkippingAdjustments(
        IntPtr handle, Int64 sequenceId, IntPtr outIsSkippingAdjustments);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetBeginOffset")]
    private static extern Status BarelySequence_SetBeginOffset(IntPtr handle, Int64 sequenceId,
                                                               double beginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetBeginPosition")]
    private static extern Status BarelySequence_SetBeginPosition(IntPtr handle, Int64 sequenceId,
                                                                 double beginPosition);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetEndPosition")]
    private static extern Status BarelySequence_SetEndPosition(IntPtr handle, Int64 sequenceId,
                                                               double endPosition);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetInstrument")]
    private static extern Status BarelySequence_SetInstrument(IntPtr handle, Int64 sequenceId,
                                                              Int64 instrumentId);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetLoopBeginOffset")]
    private static extern Status BarelySequence_SetLoopBeginOffset(IntPtr handle, Int64 sequenceId,
                                                                   double loopBeginOffset);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetLoopLength")]
    private static extern Status BarelySequence_SetLoopLength(IntPtr handle, Int64 sequenceId,
                                                              double loopLength);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetLooping")]
    private static extern Status BarelySequence_SetLooping(IntPtr handle, Int64 sequenceId,
                                                           bool isLooping);

    [DllImport(pluginName, EntryPoint = "BarelySequence_SetSkippingAdjustments")]
    private static extern Status BarelySequence_SetSkippingAdjustments(IntPtr handle,
                                                                       Int64 sequenceId,
                                                                       bool isSkippingAdjustments);
  }
}
