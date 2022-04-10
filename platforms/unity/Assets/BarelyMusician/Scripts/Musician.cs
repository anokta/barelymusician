using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Barely {
  /// Class that wraps musician.
  public static class Musician {
    /// Adjust note callback.
    ///
    /// @param definition Reference to mutable note definition.
    public delegate void AdjustNoteCallback(ref Note.Definition definition);
    public static event AdjustNoteCallback OnAdjustNote;

    /// Beat callback.
    ///
    /// @param position Beat position in beats.
    /// @param dspTime Beat time in seconds.
    public delegate void BeatCallback(double position, double dspTime);
    public static event BeatCallback OnBeat;

    /// Denotes whether musician is currently playing or not.
    public static bool IsPlaying { get; private set; }

    /// Playback position in beats.
    public static double Position {
      get { return _position; }
      set {
        if (_position != value) {
          Native.Musician_SetPosition(value);
          _position = Native.Musician_GetPosition();
        }
      }
    }
    private static double _position = 0.0;

    /// Root note pitch.
    public static double RootPitch {
      get { return _rootPitch; }
      set {
        if (_rootPitch != value) {
          Native.Musician_SetRootNote(value);
          _rootPitch = Native.Musician_GetRootNote();
        }
      }
    }
    private static double _rootPitch = 0.0;

    /// List of scale note pitches.
    public static double[] ScalePitches {
      get { return _scalePitches; }
      set {
        if (_scalePitches != value) {
          Native.Musician_SetScale(value);
          _scalePitches = Native.Musician_GetScale();
        }
      }
    }
    private static double[] _scalePitches = null;

    /// Playback tempo in bpm.
    public static double Tempo {
      get { return _tempo; }
      set {
        if (_tempo != value) {
          Native.Musician_SetTempo(value);
          _tempo = Native.Musician_GetTempo();
        }
      }
    }
    private static double _tempo = 120.0;

    /// Pauses playback.
    public static void Pause() {
      Native.Musician_Stop();
      IsPlaying = Native.Musician_IsPlaying();
    }

    /// Starts playback.
    public static void Play() {
      Native.Musician_Start();
      IsPlaying = Native.Musician_IsPlaying();
    }

    /// Stops playback.
    public static void Stop() {
      Pause();
      Position = 0.0;
    }

    /// Class that wraps native api.
    public static class Native {
      /// Invalid identifier.
      public const Int64 InvalidId = -1;

      /// Creates new instrument.
      ///
      /// @param instrument Instrument.
      /// @param noteOffCallback Reference to note off callback.
      /// @param noteOnCallback Reference to note on callback.
      /// @return Instrument identifier.
      public static Int64 Instrument_Create(Instrument instrument,
                                            ref Instrument.NoteOffCallback noteOffCallback,
                                            ref Instrument.NoteOnCallback noteOnCallback) {
        Int64 instrumentId = InvalidId;
        Int32 frameRate = AudioSettings.outputSampleRate;
        Status status = Status.Unimplemented;
        switch (instrument) {
          case SynthInstrument synth:
            status =
                BarelyInstrument_CreateOfType(Handle, InstrumentType.Synth, frameRate, _int64Ptr);
            break;
          default:
            Debug.LogError("Unsupported instrument type: " + instrument.GetType());
            return InvalidId;
        }
        if (IsOk(status)) {
          instrumentId = Marshal.ReadInt64(_int64Ptr);
          BarelyInstrument_SetNoteOffCallback(
              _handle, instrumentId, Marshal.GetFunctionPointerForDelegate(noteOffCallback),
              IntPtr.Zero);
          BarelyInstrument_SetNoteOnCallback(_handle, instrumentId,
                                             Marshal.GetFunctionPointerForDelegate(noteOnCallback),
                                             IntPtr.Zero);
        } else {
          Debug.LogError("Failed to create instrument '" + instrument.name + "': " + status);
        }
        return instrumentId;
      }

      /// Destroys instrument.
      ///
      /// @param instrument Instrument.
      public static void Instrument_Destroy(Instrument instrument) {
        Status status = BarelyInstrument_Destroy(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to destroy instrument '" + instrument.name + "': " + status);
        }
      }

      /// Returns instrument parameter value.
      ///
      /// @param instrument Instrument.
      /// @param index Parameter index.
      /// @return Parameter value.
      public static double Instrument_GetParameter(Instrument instrument, int index) {
        Status status = BarelyInstrument_GetParameter(Handle, instrument.Id, index, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument parameter " + index + " for '" +
                         instrument.name + "': " + status);
        }
        return 0.0;
      }

      /// Returns whether instrument note is playing or not.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitchk.
      /// @return True if playing, false otherwise.
      public static bool Instrument_IsNoteOn(Instrument instrument, double pitch) {
        Status status = BarelyInstrument_IsNoteOn(Handle, instrument.Id, pitch, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get instrument note " + pitch + " for '" + instrument.name +
                         "': " + status);
        }
        return false;
      }

      /// Processes next instrument output buffer.
      ///
      /// @param instrument Instrument.
      /// @param output Output buffer.
      /// @param numOutputChannels Number of output channels.
      public static void Instrument_Process(Instrument instrument, float[] output,
                                            int numOutputChannels) {
        Status status =
            BarelyInstrument_Process(Handle, instrument.Id, _output, numOutputChannels,
                                     output.Length / numOutputChannels, AudioSettings.dspTime);
        if (IsOk(status)) {
          for (int i = 0; i < output.Length; ++i) {
            output[i] = (float)_output[i];
          }
        } else {
          if (_handle != IntPtr.Zero) {
            Debug.LogError("Failed to process instrument '" + instrument.name + "': " + status);
          }
          for (int i = 0; i < output.Length; ++i) {
            output[i] = 0.0f;
          }
        }
      }

      /// Resets all instrument parameters to default value.
      ///
      /// @param instrument Instrument.
      public static void Instrument_ResetAllParameters(Instrument instrument) {
        Status status = BarelyInstrument_ResetAllParameters(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset all instrument parameters for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Resets instrument parameter to default value.
      ///
      /// @param instrument Instrument.
      /// @param index Parameter index.
      public static void Instrument_ResetParameter(Instrument instrument, int index) {
        Status status = BarelyInstrument_ResetParameter(Handle, instrument.Id, index);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to reset instrument parameter " + index + " for '" +
                         instrument.name + "': " + status);
        }
      }

      /// Sets instrument parameter value.
      ///
      /// @param instrument Instrument.
      /// @param index Parameter index.
      /// @param value Parameter value.
      public static void Instrument_SetParameter(Instrument instrument, int index, double value) {
        Status status = BarelyInstrument_SetParameter(Handle, instrument.Id, index, value);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set instrument parameter " + index + " for '" +
                         instrument.name + "': " + status);
        }
      }

      /// Starts instrument note.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @param intensity Note intensity.
      public static void Instrument_StartNote(Instrument instrument, double pitch,
                                              double intensity) {
        Status status = BarelyInstrument_StartNote(Handle, instrument.Id, pitch, intensity);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to start instrument note " + pitch + " for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Stops all instrument notes.
      ///
      /// @param instrument Instrument.
      /// @return True if success, false otherwise.
      public static void Instrument_StopAllNotes(Instrument instrument) {
        Status status = BarelyInstrument_StopAllNotes(Handle, instrument.Id);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop all instrument notes for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Stops instrument note.
      ///
      /// @param instrument Instrument.
      /// @param pitch Note pitch.
      /// @return True if success, false otherwise.
      public static void Instrument_StopNote(Instrument instrument, double pitch) {
        Status status = BarelyInstrument_StopNote(Handle, instrument.Id, pitch);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop instrument note " + pitch + " for '" + instrument.name +
                         "': " + status);
        }
      }

      /// Returns musician position.
      ///
      /// @return Position in beats.
      public static double Musician_GetPosition() {
        Status status = BarelyMusician_GetPosition(Handle, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician position: " + status);
        }
        return 0.0;
      }

      /// Returns musician root note.
      ///
      /// @return Root note pitch.
      public static double Musician_GetRootNote() {
        Status status = BarelyMusician_GetRootNote(Handle, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician root note: " + status);
        }
        return 0.0;
      }

      /// Returns musician scale.
      ///
      /// @return List of scale note pitches.
      public static double[] Musician_GetScale() {
        Status status = BarelyMusician_GetScale(Handle, _intPtrPtr, _int32Ptr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<double[]>(_intPtrPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician scale: " + status);
        }
        return null;
      }

      /// Returns musician tempo.
      ///
      /// @return Tempo in bpm.
      public static double Musician_GetTempo() {
        Status status = BarelyMusician_GetTempo(Handle, _doublePtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Double>(_doublePtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician tempo: " + status);
        }
        return 0.0;
      }

      /// Returns whether musician is currently playing or not.
      ///
      /// @return True if playing, false otherwise.
      public static bool Musician_IsPlaying() {
        Status status = BarelyMusician_IsPlaying(Handle, _booleanPtr);
        if (IsOk(status)) {
          return Marshal.PtrToStructure<Boolean>(_booleanPtr);
        } else if (_handle != IntPtr.Zero) {
          Debug.LogError("Failed to get musician playback: " + status);
        }
        return false;
      }

      /// Sets musician position.
      ///
      /// @param position Position in beats.
      public static void Musician_SetPosition(double position) {
        Status status = BarelyMusician_SetPosition(Handle, position);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician position: " + status);
        }
      }

      /// Sets musician root note.
      ///
      /// @param rootPitch Root note pitch.
      public static void Musician_SetRootNote(double rootPitch) {
        Status status = BarelyMusician_SetRootNote(Handle, rootPitch);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician root note: " + status);
        }
      }

      /// Sets musician scale.
      ///
      /// @param scalePitches List of scale note pitches.
      public static void Musician_SetScale(double[] scalePitches) {
        Status status = BarelyMusician_SetScale(Handle, scalePitches, scalePitches.Length);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician scale: " + status);
        }
      }

      /// Sets musician tempo.
      ///
      /// @param tempo Tempo in bpm.
      public static void Musician_SetTempo(double tempo) {
        Status status = BarelyMusician_SetTempo(Handle, tempo);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set musician tempo: " + status);
        }
      }

      /// Starts musician playback.
      public static void Musician_Start() {
        Status status = BarelyMusician_Start(Handle);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to start musician playback: " + status);
        }
      }

      /// Stops musician playback.
      public static void Musician_Stop() {
        Status status = BarelyMusician_Stop(Handle);
        if (!IsOk(status) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to stop musician playback: " + status);
        }
      }

      /// Creates new sequence note.
      ///
      /// @param sequence Sequence.
      /// @param definition Note definition.
      /// @param position Note position.
      /// @return Note identifier.
      public static Int64 Note_Create(Sequence sequence, Note.Definition definition,
                                      double position) {
        Int64 noteId = InvalidId;
        Status status = BarelyNote_Create(Handle, sequence.Id, definition, position, _int64Ptr);
        if (IsOk(status)) {
          noteId = Marshal.ReadInt64(_int64Ptr);
        } else {
          Debug.LogError("Failed to add sequence note to '" + sequence.name + "': " + status);
        }
        return noteId;
      }

      /// Destroys note.
      ///
      /// @param sequence Sequence.
      /// @param note Note.
      public static void Note_Destroy(Sequence sequence, Note note) {
        BarelyNote_Destroy(Handle, sequence.Id, note.Id);
      }

      /// Sets note definition.
      ///
      /// @param sequence Sequence.
      /// @param note Note.
      /// @param definition Definition.
      public static void Note_SetDefinition(Sequence sequence, Note note,
                                            Note.Definition definition) {
        BarelyNote_SetDefinition(Handle, sequence.Id, note.Id, definition);
      }

      /// Sets note position.
      ///
      /// @param sequence Sequence.
      /// @param note Note.
      /// @param position Position.
      public static void Note_SetPosition(Sequence sequence, Note note, double position) {
        BarelyNote_SetPosition(Handle, sequence.Id, note.Id, position);
      }

      /// Creates new sequence.
      ///
      /// @param sequence Sequence.
      /// @return Sequence identifier.
      public static Int64 Sequence_Create(Sequence sequence) {
        Int64 sequenceId = InvalidId;
        Status status = BarelySequence_Create(Handle, _int64Ptr);
        if (IsOk(status)) {
          sequenceId = Marshal.ReadInt64(_int64Ptr);
        } else {
          Debug.LogError("Failed to add sequence '" + sequence.name + "': " + status);
        }
        return sequenceId;
      }

      /// Destroys sequence.
      ///
      /// @param sequence Sequence.
      public static void Sequence_Destroy(Sequence sequence) {
        BarelySequence_Destroy(Handle, sequence.Id);
      }

      /// Updates sequence.
      ///
      /// @param sequence Sequence.
      // TODO(#85): This is a POC implementation only.
      public static void Sequence_Update(Sequence sequence, bool changed) {
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
          definition.pitch.type = Note.PitchType.ScaleIndex;
          for (int i = 0; i < sequence.NativeNotes.Length; ++i) {
            definition.duration = sequence.Notes[i].note.Duration;
            definition.pitch.value.scaleIndex = sequence.Notes[i].note.Pitch;
            definition.intensity = sequence.Notes[i].note.Intensity;
            sequence.NativeNotes[i] = new Note(sequence, definition, sequence.Notes[i].position);
          }
        }
      }

      // Instrument type.
      private enum InstrumentType {
        // Synth instrument.
        Synth = 0,
        // Percussion instrument.
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

      // Returns whether status is okay or not.
      private static bool IsOk(Status status) {
        return (status == Status.Ok);
      }

      // Singleton musician handle.
      private static IntPtr Handle {
        get {
          if (_isShuttingDown) {
            return IntPtr.Zero;
          }
          if (_handle == IntPtr.Zero) {
            var state =
                new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<State>();
            GameObject.DontDestroyOnLoad(state.gameObject);
            if (_handle == IntPtr.Zero) {
              GameObject.DestroyImmediate(state.gameObject);
            }
          }
          return _handle;
        }
      }
      private static IntPtr _handle = IntPtr.Zero;

      // Adjust note callback.
      private static AdjustNoteCallback _adjustNoteCallback = null;

      // Beat callback.
      private static BeatCallback _beatCallback = null;

      // `Boolean` type pointer.
      private static IntPtr _booleanPtr = IntPtr.Zero;

      // `Double` type pointer.
      private static IntPtr _doublePtr = IntPtr.Zero;

      // `Int32` type pointer.
      private static IntPtr _int32Ptr = IntPtr.Zero;

      // `Int64` type pointer.
      private static IntPtr _int64Ptr = IntPtr.Zero;

      // `IntPtr` type pointer.
      private static IntPtr _intPtrPtr = IntPtr.Zero;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Latency in seconds.
      private static double _latency = 0.0;

      // Internal output buffer.
      private static double[] _output = null;

      // Component that manages internal state.
      private class State : MonoBehaviour {
        private void Awake() {
          AllocatePtrs();
          AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
          Status status = BarelyMusician_Create(_intPtrPtr);
          if (!IsOk(status)) {
            Debug.LogError("Failed to initialize BarelyMusician: " + status);
            return;
          }
          _handle = Marshal.PtrToStructure<IntPtr>(_intPtrPtr);
          _adjustNoteCallback = delegate(ref Note.Definition definition) {
            OnAdjustNote?.Invoke(ref definition);
          };
          _beatCallback = delegate(double position, double dspTime) {
            OnBeat?.Invoke(position, dspTime);
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
          DeallocatePtrs();
        }

        private void OnApplicationQuit() {
          _isShuttingDown = true;
          BarelyMusician_Destroy(_handle);
          _handle = IntPtr.Zero;
          DeallocatePtrs();
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

        // Allocates unmanaged memory for native calls.
        private void AllocatePtrs() {
          _booleanPtr = Marshal.AllocHGlobal(Marshal.SizeOf<Boolean>());
          _doublePtr = Marshal.AllocHGlobal(Marshal.SizeOf<Double>());
          _int32Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int32>());
          _int64Ptr = Marshal.AllocHGlobal(Marshal.SizeOf<Int64>());
          _intPtrPtr = Marshal.AllocHGlobal(Marshal.SizeOf<IntPtr>());
        }

        // Deallocates unmanaged memory for native calls.
        private void DeallocatePtrs() {
          if (_booleanPtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_booleanPtr);
            _booleanPtr = IntPtr.Zero;
          }
          if (_doublePtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_doublePtr);
            _doublePtr = IntPtr.Zero;
          }
          if (_int32Ptr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_int32Ptr);
            _int32Ptr = IntPtr.Zero;
          }
          if (_int64Ptr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_int64Ptr);
            _int64Ptr = IntPtr.Zero;
          }
          if (_intPtrPtr != IntPtr.Zero) {
            Marshal.FreeHGlobal(_intPtrPtr);
            _intPtrPtr = IntPtr.Zero;
          }
        }
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
                                                            Int32 numOutputFrames,
                                                            double timestamp);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetAllParameters")]
      private static extern Status BarelyInstrument_ResetAllParameters(IntPtr handle,
                                                                       Int64 instrumentId);

      [DllImport(pluginName, EntryPoint = "BarelyInstrument_ResetParameter")]
      private static extern Status BarelyInstrument_ResetParameter(IntPtr handle,
                                                                   Int64 instrumentId, Int32 index);

      // TODO(#105): Implement `Instrument.SetData` using this.
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

      [DllImport(pluginName, EntryPoint = "BarelyMusician_GetScale")]
      private static extern Status BarelyMusician_GetScale(IntPtr handle, IntPtr outScalePitches,
                                                           IntPtr outNumScalePitches);

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
      private static extern Status BarelyMusician_SetScale(IntPtr handle,
                                                           [In] double[] scalePitches,
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
      private static extern Status BarelyNote_Destroy(IntPtr handle, Int64 sequenceId,
                                                      Int64 noteId);

      [DllImport(pluginName, EntryPoint = "BarelyNote_GetDefinition")]
      private static extern Status BarelyNote_GetDefinition(IntPtr handle, Int64 sequenceId,
                                                            Int64 noteId, IntPtr outDefinition);

      [DllImport(pluginName, EntryPoint = "BarelyNote_GetPosition")]
      private static extern Status BarelyNote_GetPosition(IntPtr handle, Int64 sequenceId,
                                                          Int64 noteId, IntPtr outPosition);

      [DllImport(pluginName, EntryPoint = "BarelyNote_SetDefinition")]
      private static extern Status BarelyNote_SetDefinition(IntPtr handle, Int64 sequenceId,
                                                            Int64 noteId,
                                                            Note.Definition definition);

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
      private static extern Status BarelySequence_GetLoopBeginOffset(IntPtr handle,
                                                                     Int64 sequenceId,
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
      private static extern Status BarelySequence_SetLoopBeginOffset(IntPtr handle,
                                                                     Int64 sequenceId,
                                                                     double loopBeginOffset);

      [DllImport(pluginName, EntryPoint = "BarelySequence_SetLoopLength")]
      private static extern Status BarelySequence_SetLoopLength(IntPtr handle, Int64 sequenceId,
                                                                double loopLength);

      [DllImport(pluginName, EntryPoint = "BarelySequence_SetLooping")]
      private static extern Status BarelySequence_SetLooping(IntPtr handle, Int64 sequenceId,
                                                             bool isLooping);

      [DllImport(pluginName, EntryPoint = "BarelySequence_SetSkippingAdjustments")]
      private static extern Status BarelySequence_SetSkippingAdjustments(
          IntPtr handle, Int64 sequenceId, bool isSkippingAdjustments);
    }
  }
}
