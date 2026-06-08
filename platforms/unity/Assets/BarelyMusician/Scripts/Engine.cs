using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Xml.Serialization;
using UnityEngine;

namespace Barely {
  /// A representation of an engine that governs all musical components.
  public static class Engine {
    /// Gain.
    public static float Gain {
      get { return _gain; }
      set {
        if (_gain != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.GAIN, value);
          _gain = value;
        }
      }
    }
    private static float _gain = 1.0f;

    /// Compressor mix.
    public static float CompMix {
      get { return _compMix; }
      set {
        if (_compMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMP_MIX, value);
          _compMix = value;
        }
      }
    }
    private static float _compMix = 1.0f;

    /// Compressor attack.
    public static float CompAttack {
      get { return _compAttack; }
      set {
        if (_compAttack != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMP_ATTACK, value);
          _compAttack = value;
        }
      }
    }
    private static float _compAttack = 0.0f;

    /// Compressor release.
    public static float CompRelease {
      get { return _compRelease; }
      set {
        if (_compRelease != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMP_RELEASE, value);
          _compRelease = value;
        }
      }
    }
    private static float _compRelease = 0.0f;

    /// Normalized compressor threshold in logarithmic scale.
    public static float CompThreshold {
      get { return _compThreshold; }
      set {
        if (_compThreshold != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMP_THRESHOLD, value);
          _compThreshold = value;
        }
      }
    }
    private static float _compThreshold = 1.0f;

    /// Compressor ratio.
    public static float CompRatio {
      get { return _compRatio; }
      set {
        if (_compRatio != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.COMP_RATIO, value);
          _compRatio = value;
        }
      }
    }
    private static float _compRatio = 0.0f;

    /// Delay mix.
    public static float DelayMix {
      get { return _delayMix; }
      set {
        if (_delayMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_MIX, value);
          _delayMix = value;
        }
      }
    }
    private static float _delayMix = 1.0f;

    /// Delay time in seconds.
    public static float DelayTime {
      get { return _delayTime; }
      set {
        if (_delayTime != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_TIME, value);
          _delayTime = value;
        }
      }
    }
    private static float _delayTime = 0.0f;

    /// Delay feedback.
    public static float DelayFeedback {
      get { return _delayFeedback; }
      set {
        if (_delayFeedback != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_FEEDBACK, value);
          _delayFeedback = value;
        }
      }
    }
    private static float _delayFeedback = 0.0f;

    /// Delay low-pass filter cutoff.
    public static float DelayLpfCutoff {
      get { return _delayLpfCutoff; }
      set {
        if (_delayLpfCutoff != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_LPF_CUTOFF, value);
          _delayLpfCutoff = value;
        }
      }
    }
    private static float _delayLpfCutoff = 1.0f;

    /// Delay high-pass filter cutoff.
    public static float DelayHpfCutoff {
      get { return _delayHpfCutoff; }
      set {
        if (_delayHpfCutoff != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_HPF_CUTOFF, value);
          _delayHpfCutoff = value;
        }
      }
    }
    private static float _delayHpfCutoff = 0.0f;

    /// Delay ping-pong.
    public static float DelayPingPong {
      get { return _delayPingPong; }
      set {
        if (_delayPingPong != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_PING_PONG, value);
          _delayPingPong = value;
        }
      }
    }
    private static float _delayPingPong = 0.0f;

    /// Delay reverb send.
    public static float DelayReverbSend {
      get { return _delayReverbSend; }
      set {
        if (_delayReverbSend != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.DELAY_REVERB_SEND, value);
          _delayReverbSend = value;
        }
      }
    }
    private static float _delayReverbSend = 0.0f;

    /// Reverb mix.
    public static float ReverbMix {
      get { return _reverbMix; }
      set {
        if (_reverbMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.REVERB_MIX, value);
          _reverbMix = value;
        }
      }
    }
    private static float _reverbMix = 1.0f;

    /// Reverb damping ratio.
    public static float ReverbDamping {
      get { return _reverbDamping; }
      set {
        if (_reverbDamping != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.REVERB_DAMPING, value);
          _reverbDamping = value;
        }
      }
    }
    private static float _reverbDamping = 0.0f;

    /// Reverb room size.
    public static float ReverbRoomSize {
      get { return _reverbRoomSize; }
      set {
        if (_reverbRoomSize != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.REVERB_ROOM_SIZE, value);
          _reverbRoomSize = value;
        }
      }
    }
    private static float _reverbRoomSize = 0.0f;

    /// Reverb stereo width.
    public static float ReverbStereoWidth {
      get { return _reverbStereoWidth; }
      set {
        if (_reverbStereoWidth != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.REVERB_STEREO_WIDTH, value);
          _reverbStereoWidth = value;
        }
      }
    }
    private static float _reverbStereoWidth = 1.0f;

    /// Reverb freeze.
    public static bool ReverbFreeze {
      get { return _reverbFreeze; }
      set {
        if (_reverbFreeze != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.REVERB_FREEZE, value ? 1.0f : 0.0f);
          _reverbFreeze = value;
        }
      }
    }
    private static bool _reverbFreeze = false;

    /// Sidechain mix.
    public static float SidechainMix {
      get { return _sidechainMix; }
      set {
        if (_sidechainMix != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_MIX, value);
          _sidechainMix = value;
        }
      }
    }
    private static float _sidechainMix = 1.0f;

    /// Sidechain attack.
    public static float SidechainAttack {
      get { return _sidechainAttack; }
      set {
        if (_sidechainAttack != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_ATTACK, value);
          _sidechainAttack = value;
        }
      }
    }
    private static float _sidechainAttack = 0.0f;

    /// Sidechain release.
    public static float SidechainRelease {
      get { return _sidechainRelease; }
      set {
        if (_sidechainRelease != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_RELEASE, value);
          _sidechainRelease = value;
        }
      }
    }
    private static float _sidechainRelease = 0.0f;

    /// Sidechain threshold.
    public static float SidechainThreshold {
      get { return _sidechainThreshold; }
      set {
        if (_sidechainThreshold != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_THRESHOLD, value);
          _sidechainThreshold = value;
        }
      }
    }
    private static float _sidechainThreshold = 1.0f;

    /// Sidechain ratio.
    public static float SidechainRatio {
      get { return _sidechainRatio; }
      set {
        if (_sidechainRatio != value) {
          Internal.Engine_SetControl(Internal.EngineControlType.SIDECHAIN_RATIO, value);
          _sidechainRatio = value;
        }
      }
    }
    private static float _sidechainRatio = 0.0f;

    /// Tempo in beats per minute.
    public static double Tempo {
      get { return _tempo; }
      set {
        if (_tempo != value) {
          Internal.Engine_SetTempo(value);
          _tempo = value;
        }
      }
    }
    private static double _tempo = 120.0;

    /// Timestamp in seconds.
    public static double Timestamp {
      get { return Internal.Engine_GetTimestamp(); }
    }

    public static class Internal {
      public enum InstrumentControlType {
        // Normalized gain in logarithmic scale.
        [InspectorName("Gain")] GAIN = 0,
        // Pitch shift.
        [InspectorName("Pitch Shift")] PITCH_SHIFT,
        // Stereo pan.
        [InspectorName("Stereo Pan")] STEREO_PAN,
        // Envelope attack in seconds.
        [InspectorName("Attack")] ATTACK,
        // Envelope decay in seconds.
        [InspectorName("Decay")] DECAY,
        // Envelope sustain.
        [InspectorName("Sustain")] SUSTAIN,
        // Envelope release in seconds.
        [InspectorName("Release")] RELEASE,
        // Slice playback mode.
        [InspectorName("Slice Mode")] SLICE_MODE,
        // Oscillator mix.
        [InspectorName("Osc Mix")] OSC_MIX,
        // Oscillator mode.
        [InspectorName("Osc Mode")] OSC_MODE,
        // Oscillator noise mix.
        [InspectorName("Osc Noise Mix")] OSC_NOISE_MIX,
        // Oscillator pitch shift.
        [InspectorName("Osc Pitch Shift")] OSC_PITCH_SHIFT,
        // Oscillator shape.
        [InspectorName("Osc Shape")] OSC_SHAPE,
        // Oscillator skew.
        [InspectorName("Osc Skew")] OSC_SKEW,
        // Bit crusher depth.
        [InspectorName("Crush Depth")] CRUSH_DEPTH,
        // Bit crusher rate.
        [InspectorName("Crush Rate")] CRUSH_RATE,
        // Distortion mix.
        [InspectorName("Distortion Mix")] DISTORTION_MIX,
        // Distortion drive.
        [InspectorName("Distortion Drive")] DISTORTION_DRIVE,
        // Filter cutoff.
        [InspectorName("Filter Cutoff")] FILTER_CUTOFF,
        // Filter resonance.
        [InspectorName("Filter Resonance")] FILTER_RESONANCE,
        // Filter tone.
        [InspectorName("Filter Tone")] FILTER_TONE,
        // Delay send.
        [InspectorName("Delay Send")] DELAY_SEND,
        // Reverb send.
        [InspectorName("Reverb Send")] REVERB_SEND,
        // Sidechain send.
        [InspectorName("Sidechain Send")] SIDECHAIN_SEND,
        // Retrigger.
        [InspectorName("Retrigger")] RETRIGGER,
        // Number of voices.
        [InspectorName("Voice Count")] VOICE_COUNT,
      }

      public enum EngineControlType {
        // Gain.
        [InspectorName("Gain")] GAIN = 0,
        // Compressor mix.
        [InspectorName("Comp Mix")] COMP_MIX,
        // Compressor attack in seconds.
        [InspectorName("Comp Attack")] COMP_ATTACK,
        // Compressor release in seconds.
        [InspectorName("Comp Release")] COMP_RELEASE,
        // Normalized compressor threshold in logarithmic scale.
        [InspectorName("Comp Threshold")] COMP_THRESHOLD,
        // Compressor ratio.
        [InspectorName("Comp Ratio")] COMP_RATIO,
        // Delay mix.
        [InspectorName("Delay Mix")] DELAY_MIX,
        // Delay time in seconds.
        [InspectorName("Delay Time")] DELAY_TIME,
        // Delay feedback.
        [InspectorName("Delay Feedback")] DELAY_FEEDBACK,
        // Delay low-pass filter cutoff.
        [InspectorName("Delay LPF Cutoff")] DELAY_LPF_CUTOFF,
        // Delay high-pass filter cutoff.
        [InspectorName("Delay HPF Cutoff")] DELAY_HPF_CUTOFF,
        // Delay ping-pong.
        [InspectorName("Delay Ping-Pong")] DELAY_PING_PONG,
        // Delay reverb send.
        [InspectorName("Delay Reverb Send")] DELAY_REVERB_SEND,
        // Reverb mix.
        [InspectorName("Reverb Mix")] REVERB_MIX,
        // Reverb damping ratio.
        [InspectorName("Reverb Damping")] REVERB_DAMPING,
        // Reverb room size.
        [InspectorName("Reverb Room Size")] REVERB_ROOM_SIZE,
        // Reverb stereo width.
        [InspectorName("Reverb Stereo Width")] REVERB_STEREO_WIDTH,
        // Reverb freeze.
        [InspectorName("Reverb Freeze")] REVERB_FREEZE,
        // Sidechain mix.
        [InspectorName("Sidechain Mix")] SIDECHAIN_MIX,
        // Sidechain attack in seconds.
        [InspectorName("Sidechain Attack")] SIDECHAIN_ATTACK,
        // Sidechain release in seconds.
        [InspectorName("Sidechain Release")] SIDECHAIN_RELEASE,
        // Sidechain threshold.
        [InspectorName("Sidechain Threshold")] SIDECHAIN_THRESHOLD,
        // Sidechain ratio.
        [InspectorName("Sidechain Ratio")] SIDECHAIN_RATIO,
      }

      public enum NoteControlType {
        // Gain.
        [InspectorName("Gain")] GAIN = 0,
        // Pitch shift.
        [InspectorName("Pitch Shift")] PITCH_SHIFT,
      }

      public static double Engine_GetTimestamp() {
        double timestamp = 0.0;
        if (!BarelyEngine_GetTimestamp(Handle, ref timestamp) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to get engine timestamp");
        }
        return timestamp;
      }

      public static void Engine_SetControl(EngineControlType type, float value) {
        if (!BarelyEngine_SetControl(Handle, type, value) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set engine engine control");
        }
      }

      public static void Engine_SetTempo(double tempo) {
        if (!BarelyEngine_SetTempo(Handle, tempo) && _handle != IntPtr.Zero) {
          Debug.LogError("Failed to set engine tempo");
        }
      }

      public static void Instrument_Create(Instrument instrument, ref UInt32 instrumentId) {
#if UNITY_EDITOR
        if (!Application.isPlaying) {
          _isShuttingDown = false;
        }
#endif  // UNITY_EDITOR
        if (Handle == IntPtr.Zero || _handle != IntPtr.Zero && instrumentId > 0) {
          return;
        }
        if (!BarelyEngine_CreateInstrument(_handle, ref instrumentId)) {
          Debug.LogError("Failed to create instrument '" + instrument.name + "'");
          return;
        }
        _instruments.Add(instrumentId, instrument);
      }

      public static void Instrument_Destroy(ref UInt32 instrumentId) {
        if (Handle == IntPtr.Zero || instrumentId == 0) {
          instrumentId = 0;
          return;
        }
        if (!BarelyInstrument_Destroy(_handle, instrumentId)) {
          Debug.LogError("Failed to destroy instrument");
        }
        _instruments.Remove(instrumentId);
        _slices.Remove(instrumentId);
        instrumentId = 0;
      }

      public static void Instrument_SetControl(UInt32 instrumentId, InstrumentControlType type,
                                               float value) {
        if (!BarelyInstrument_SetControl(Handle, instrumentId, type, value) &&
            _handle != IntPtr.Zero && instrumentId > 0) {
          Debug.LogError("Failed to set instrument control " + type + " value to " + value);
        }
      }

      public static void Instrument_SetNoteControl(UInt32 instrumentId, float pitch,
                                                   NoteControlType type, float value) {
        if (!BarelyInstrument_SetNoteControl(Handle, instrumentId, pitch, type, value) &&
            _handle != IntPtr.Zero && instrumentId > 0) {
          Debug.LogError("Failed to set instrument note " + pitch + " control " + type +
                         " value to " + value);
        }
      }

      public static void Instrument_SetNoteOff(UInt32 instrumentId, float pitch) {
        if (!BarelyInstrument_SetNoteOff(Handle, instrumentId, pitch) && _handle != IntPtr.Zero &&
            instrumentId > 0) {
          Debug.LogError("Failed to stop instrument note " + pitch + "");
        }
      }

      public static void Instrument_SetNoteOn(UInt32 instrumentId, float pitch) {
        if (!BarelyInstrument_SetNoteOn(Handle, instrumentId, pitch) && _handle != IntPtr.Zero &&
            instrumentId > 0) {
          Debug.LogError("Failed to start instrument note " + pitch);
        }
      }

      public static void Instrument_SetSampleData(UInt32 instrumentId,
                                                  List<Instrument.Slice> instrumentSlices) {
        Slice[] slices = null;
        if (instrumentSlices.Count > 0) {
          slices = new Slice[instrumentSlices.Count];
          for (int i = 0; i < slices.Length; ++i) {
            if (instrumentSlices[i].Sample != null) {
              instrumentSlices[i].Sample.LoadAudioData();
            }
            slices[i] = new Slice() {
              samples = instrumentSlices[i].Data,
              sampleCount =
                  (instrumentSlices[i].Data != null) ? instrumentSlices[i].Sample.samples : 0,
              sampleRate =
                  (instrumentSlices[i].Data != null) ? instrumentSlices[i].Sample.frequency : 0,
              rootPitch = instrumentSlices[i].RootPitch / 12.0f,
            };
          }
        }
        if (!BarelyInstrument_SetSampleData(Handle, instrumentId, slices, instrumentSlices.Count) &&
            _handle != IntPtr.Zero && instrumentId > 0) {
          Debug.LogError("Failed to set instrument sample data");
        }
        if (_slices != null) {
          _slices[instrumentId] = new List<float[]>();
          for (int i = 0; i < slices.Length; ++i) {
            _slices[instrumentId].Add(slices[i].samples);
          }
        } else {
          _slices.Remove(instrumentId);
        }
      }

      public static void Performer_Create(Performer performer, ref UInt32 performerId) {
        if (Handle == IntPtr.Zero || _handle != IntPtr.Zero && performerId > 0) {
          return;
        }
        if (!BarelyEngine_CreatePerformer(_handle, ref performerId)) {
          Debug.LogError("Failed to create performer '" + performer.name + "'");
          return;
        }
        _performers.Add(performerId, performer);
      }

      public static void Performer_Destroy(ref UInt32 performerId) {
        if (Handle == IntPtr.Zero || performerId == 0) {
          performerId = 0;
          return;
        }
        if (!BarelyPerformer_Destroy(_handle, performerId)) {
          Debug.LogError("Failed to destroy performer");
        }
        _performers.Remove(performerId);
        performerId = 0;
      }

      public static double Performer_GetPosition(UInt32 performerId) {
        double position = 0.0;
        if (!BarelyPerformer_GetPosition(_handle, performerId, ref position) &&
            _handle != IntPtr.Zero && performerId > 0) {
          Debug.LogError("Failed to get performer position");
        }
        return position;
      }

      public static void Performer_SetLoopBeginPosition(UInt32 performerId,
                                                        double loopBeginPosition) {
        if (!BarelyPerformer_SetLoopBeginPosition(_handle, performerId, loopBeginPosition) &&
            _handle != IntPtr.Zero && performerId > 0) {
          Debug.LogError("Failed to set performer loop begin position");
        }
      }

      public static void Performer_SetLoopLength(UInt32 performerId, double loopLength) {
        if (!BarelyPerformer_SetLoopLength(_handle, performerId, loopLength) &&
            _handle != IntPtr.Zero && performerId > 0) {
          Debug.LogError("Failed to set performer loop length");
        }
      }

      public static void Performer_SetLooping(UInt32 performerId, bool isLooping) {
        if (!BarelyPerformer_SetLooping(_handle, performerId, isLooping) &&
            _handle != IntPtr.Zero && performerId > 0) {
          Debug.LogError("Failed to set performer looping");
        }
      }

      public static void Performer_SetPosition(UInt32 performerId, double position) {
        if (!BarelyPerformer_SetPosition(_handle, performerId, position) &&
            _handle != IntPtr.Zero && performerId > 0) {
          Debug.LogError("Failed to set performer position");
        }
      }

      public static void Performer_Start(UInt32 performerId) {
        if (!BarelyPerformer_Start(_handle, performerId) && _handle != IntPtr.Zero &&
            performerId > 0) {
          Debug.LogError("Failed to start performer");
        }
      }

      public static void Performer_Stop(UInt32 performerId) {
        if (!BarelyPerformer_Stop(_handle, performerId) && _handle != IntPtr.Zero &&
            performerId > 0) {
          Debug.LogError("Failed to stop performer");
        }
      }

      public static void Task_Create(Task task, UInt32 performerId, double position,
                                     double duration, int priority, ref UInt32 taskId) {
        if (Handle == IntPtr.Zero || _handle != IntPtr.Zero && taskId > 0) {
          return;
        }
        if (!BarelyPerformer_CreateTask(_handle, performerId, position,
                                        Math.Max(duration, _minTaskDuration), priority,
                                        Task_OnEvent, ref taskId, ref taskId)) {
          Debug.LogError("Failed to create task '" + task + "'");
          return;
        }
        _tasks.Add(taskId, task);
        BarelyTask_SetEventCallback(_handle, taskId, Task_OnEvent, ref taskId);
      }

      public static void Task_Destroy(UInt32 performerId, ref UInt32 taskId) {
        if (Handle == IntPtr.Zero || performerId == 0 || taskId == 0) {
          taskId = 0;
          return;
        }
        if (!BarelyTask_Destroy(_handle, taskId)) {
          Debug.LogError("Failed to destroy performer task");
        }
        _tasks.Remove(taskId);
        taskId = 0;
      }

      public static bool Task_IsActive(UInt32 taskId) {
        bool isActive = false;
        if (!BarelyTask_IsActive(_handle, taskId, ref isActive) && _handle != IntPtr.Zero &&
            taskId > 0) {
          Debug.LogError("Failed to get if performer task is active");
        }
        return isActive;
      }

      public static void Task_SetDuration(UInt32 taskId, double duration) {
        if (!BarelyTask_SetDuration(_handle, taskId, Math.Max(duration, _minTaskDuration)) &&
            _handle != IntPtr.Zero && taskId > 0) {
          Debug.LogError("Failed to set performer task duration");
        }
      }

      public static void Task_SetPosition(UInt32 taskId, double position) {
        if (!BarelyTask_SetPosition(_handle, taskId, position) && _handle != IntPtr.Zero &&
            taskId > 0) {
          Debug.LogError("Failed to set performer task position");
        }
      }

      public static void Task_SetPriority(UInt32 taskId, int priority) {
        if (!BarelyTask_SetPriority(_handle, taskId, priority) && _handle != IntPtr.Zero &&
            taskId > 0) {
          Debug.LogError("Failed to set performer task priority");
        }
      }

      public static float Scale_GetPitch(Barely.Scale scale, int degree) {
        float pitch = 0.0f;
        _scale.pitches = scale.Pitches;
        _scale.pitchCount = scale.PitchCount;
        _scale.rootPitch = scale.RootPitch;
        _scale.mode = scale.Mode;
        if (!BarelyScale_GetPitch(ref _scale, degree, ref pitch)) {
          Debug.LogError("Failed to get scale " + scale + " note pitch with a degree " + degree);
        }
        return pitch;
      }

      private delegate void TaskEventCallback(TaskEventType type, ref UInt32 userData);
      [AOT.MonoPInvokeCallback(typeof(TaskEventCallback))]
      private static void Task_OnEvent(TaskEventType type, ref UInt32 userData) {
        if (_tasks.TryGetValue(userData, out var task)) {
          Task.Internal.OnProcess(task, type);
        }
      }

      [StructLayout(LayoutKind.Sequential)]
      private struct Slice {
        public float[] samples;
        public Int32 sampleCount;
        public Int32 sampleRate;
        public float rootPitch;
      }

      [StructLayout(LayoutKind.Sequential)]
      public struct BarelyEngineConfig {
        public Int32 sampleRate;
        public Int32 maxFrameCount;
        public Int32 maxInstrumentCount;
        public Int32 maxPerformerCount;
        public Int32 maxTaskCount;
        public Int32 maxNoteCount;
        public Int32 maxSliceCount;
        public Int32 maxVoiceCount;
      }

      [StructLayout(LayoutKind.Sequential)]
      private struct Scale {
        public float[] pitches;
        public Int32 pitchCount;
        public float rootPitch;
        public Int32 mode;
      }

      // Singleton engine handle.
      private static IntPtr Handle {
        get {
          if (_isShuttingDown) {
            return IntPtr.Zero;
          }
          if (_handle == IntPtr.Zero) {
            var state =
                new GameObject() { hideFlags = HideFlags.HideAndDontSave }.AddComponent<State>();
            if (Application.isPlaying) {
              GameObject.DontDestroyOnLoad(state.gameObject);
            }
            if (_handle == IntPtr.Zero) {
              GameObject.DestroyImmediate(state.gameObject);
              _isShuttingDown = true;
            }
          }
          return _handle;
        }
      }
      private static IntPtr _handle = IntPtr.Zero;

      // Minimum task duration to avoid zero duration tasks.
      private const double _minTaskDuration = 1e-6;

      // Denotes if the system is shutting down to avoid re-initialization.
      private static bool _isShuttingDown = false;

      // Memory allocation for the engine.
      private static IntPtr _allocation = IntPtr.Zero;

      private static Dictionary<UInt32, Instrument> _instruments = null;
      private static Dictionary<UInt32, List<float[]>> _slices = null;
      private static Dictionary<UInt32, Performer> _performers = null;
      private static Dictionary<UInt32, Task> _tasks = null;

      private static Scale _scale = new Scale {
        pitches = null,
        pitchCount = 0,
        rootPitch = 0.0f,
        mode = 0,
      };

      // Component that manages internal state.
      [ExecuteInEditMode]
      [RequireComponent(typeof(AudioSource))]
      private class State : MonoBehaviour {
        private void Awake() {
#if UNITY_EDITOR
          UnityEditor.AssemblyReloadEvents.beforeAssemblyReload += OnAssemblyReload;
          UnityEditor.EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
#endif  // UNITY_EDITOR
          AudioSettings.OnAudioConfigurationChanged += OnAudioConfigurationChanged;
          Initialize();
          var source = GetComponent<AudioSource>();
          source.loop = true;
          source.clip = AudioClip.Create("zeros", 64, 1, AudioSettings.outputSampleRate, false);
          source.Play();
        }

        private void OnDestroy() {
#if UNITY_EDITOR
          UnityEditor.AssemblyReloadEvents.beforeAssemblyReload -= OnAssemblyReload;
          UnityEditor.EditorApplication.playModeStateChanged -= OnPlayModeStateChanged;
#endif  // UNITY_EDITOR
          AudioSettings.OnAudioConfigurationChanged -= OnAudioConfigurationChanged;
          Shutdown();
        }

#if UNITY_EDITOR
        private void OnAssemblyReload() {
          if (Application.isPlaying) {
            GameObject.Destroy(gameObject);
          } else {
            _isShuttingDown = true;
            GameObject.DestroyImmediate(gameObject);
          }
        }

        private void OnPlayModeStateChanged(UnityEditor.PlayModeStateChange state) {
          if (state == UnityEditor.PlayModeStateChange.ExitingEditMode) {
            _isShuttingDown = true;
            GameObject.DestroyImmediate(gameObject);
          }
        }
#endif  // UNITY_EDITOR

        private void OnApplicationQuit() {
          GameObject.Destroy(gameObject);
        }

        private void OnAudioConfigurationChanged(bool deviceWasChanged) {
          Shutdown();
          var instruments = new List<Instrument>(_instruments.Values);
          for (int i = 0; i < instruments.Count; ++i) {
            instruments[i].enabled = false;
          }
          var performers = new List<Performer>(_performers.Values);
          for (int i = 0; i < performers.Count; ++i) {
            performers[i].enabled = false;
          }
          Initialize();
          for (int i = 0; i < instruments.Count; ++i) {
            instruments[i].enabled = true;
          }
          for (int i = 0; i < performers.Count; ++i) {
            performers[i].enabled = true;
          }
        }

        private void OnAudioFilterRead(float[] data, int channels) {
          BarelyEngine_Process(_handle, data, channels, data.Length / channels,
                               AudioSettings.dspTime);
        }

        private void LateUpdate() {
          BarelyEngine_Update(_handle, GetNextTimestamp());
        }

        private void Initialize() {
          _isShuttingDown = false;
          var config = AudioSettings.GetConfiguration();
          var engineConfig = new BarelyEngineConfig {
            sampleRate = config.sampleRate,
            maxFrameCount = config.dspBufferSize,
            maxInstrumentCount = 100,
            maxPerformerCount = 100,
            maxTaskCount = 5000,
            maxNoteCount = 1000,
            maxSliceCount = 1000,
            maxVoiceCount = 200,
          };
          Int32 allocationSize = 0;
          BarelyEngineConfig_GetRequiredAllocationSize(ref engineConfig, ref allocationSize);
          _allocation = Marshal.AllocHGlobal(allocationSize);
          if (!BarelyEngine_Create(ref engineConfig, _allocation, allocationSize, ref _handle)) {
            Debug.LogError("Failed to initialize BarelyEngine");
            Marshal.FreeHGlobal(_allocation);
            _allocation = IntPtr.Zero;
            return;
          }
          BarelyEngine_SetTempo(_handle, _tempo);
          BarelyEngine_SetControl(_handle, EngineControlType.COMP_MIX, _compMix);
          BarelyEngine_SetControl(_handle, EngineControlType.COMP_THRESHOLD, _compThreshold);
          BarelyEngine_SetControl(_handle, EngineControlType.COMP_ATTACK, _compAttack);
          BarelyEngine_SetControl(_handle, EngineControlType.COMP_RELEASE, _compRelease);
          BarelyEngine_SetControl(_handle, EngineControlType.COMP_RATIO, _compRatio);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_MIX, _delayMix);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_TIME, _delayTime);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_FEEDBACK, _delayFeedback);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_LPF_CUTOFF, _delayLpfCutoff);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_HPF_CUTOFF, _delayHpfCutoff);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_PING_PONG, _delayPingPong);
          BarelyEngine_SetControl(_handle, EngineControlType.DELAY_REVERB_SEND, _delayReverbSend);
          BarelyEngine_SetControl(_handle, EngineControlType.REVERB_MIX, _reverbMix);
          BarelyEngine_SetControl(_handle, EngineControlType.REVERB_DAMPING, _reverbDamping);
          BarelyEngine_SetControl(_handle, EngineControlType.REVERB_ROOM_SIZE, _reverbRoomSize);
          BarelyEngine_SetControl(_handle, EngineControlType.REVERB_STEREO_WIDTH,
                                  _reverbStereoWidth);
          BarelyEngine_SetControl(_handle, EngineControlType.REVERB_FREEZE,
                                  _reverbFreeze ? 1.0f : 0.0f);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_MIX, _sidechainMix);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_THRESHOLD,
                                  _sidechainThreshold);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_ATTACK, _sidechainAttack);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_RELEASE, _sidechainRelease);
          BarelyEngine_SetControl(_handle, EngineControlType.SIDECHAIN_RATIO, _sidechainRatio);
          _dspLatency = (float)(config.dspBufferSize + 1) / config.sampleRate;
          _instruments = new Dictionary<UInt32, Instrument>();
          _slices = new Dictionary<UInt32, List<float[]>>();
          _performers = new Dictionary<UInt32, Performer>();
          _tasks = new Dictionary<UInt32, Task>();
          BarelyEngine_Update(_handle, GetNextTimestamp());
        }

        private void Shutdown() {
          _isShuttingDown = true;
          BarelyEngine_Destroy(_handle);
          _handle = IntPtr.Zero;
          Marshal.FreeHGlobal(_allocation);
          _allocation = IntPtr.Zero;
        }

        private double GetNextTimestamp() {
          return Math.Max(AudioSettings.dspTime + Math.Max(_dspLatency, _minLookahead),
                          Timestamp + (double)Time.deltaTime);
        }

        // Minimum lookahead time, set to an empirical value that can be adjusted as needed.
        private const double _minLookahead = 0.025;
        private double _dspLatency = 0.0;
      }

#if !UNITY_EDITOR && UNITY_IOS
      private const string _pluginName = "__Internal";
#else
      private const string _pluginName = "barelymusicianunity";
#endif  // !UNITY_EDITOR && UNITY_IOS

      [DllImport(_pluginName, EntryPoint = "BarelyEngineConfig_GetRequiredAllocationSize")]
      private static extern bool BarelyEngineConfig_GetRequiredAllocationSize(
          ref BarelyEngineConfig config, ref Int32 outAllocationSize);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Create")]
      private static extern bool BarelyEngine_Create(ref BarelyEngineConfig config,
                                                     IntPtr allocation, Int32 allocationSize,
                                                     ref IntPtr outEngine);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_CreateInstrument")]
      private static extern bool BarelyEngine_CreateInstrument(IntPtr engine,
                                                               ref UInt32 outInstrumentId);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_CreatePerformer")]
      private static extern bool BarelyEngine_CreatePerformer(IntPtr engine,
                                                              ref UInt32 outPerformerId);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Destroy")]
      private static extern bool BarelyEngine_Destroy(IntPtr engine);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_GetTimestamp")]
      private static extern bool BarelyEngine_GetTimestamp(IntPtr engine, ref double outTimestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Process")]
      private static extern bool BarelyEngine_Process(IntPtr engine,
                                                      [In, Out] float[] outputSamples,
                                                      Int32 outputChannelCount,
                                                      Int32 outputFrameCount, double timestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_SetControl")]
      private static extern bool BarelyEngine_SetControl(IntPtr engine, EngineControlType type,
                                                         float value);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_SetTempo")]
      private static extern bool BarelyEngine_SetTempo(IntPtr engine, double tempo);

      [DllImport(_pluginName, EntryPoint = "BarelyEngine_Update")]
      private static extern bool BarelyEngine_Update(IntPtr engine, double timestamp);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_Destroy")]
      private static extern bool BarelyInstrument_Destroy(IntPtr engine, UInt32 instrumentId);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetControl")]
      private static extern bool BarelyInstrument_SetControl(IntPtr engine, UInt32 instrumentId,
                                                             InstrumentControlType type,
                                                             float value);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteControl")]
      private static extern bool BarelyInstrument_SetNoteControl(IntPtr engine, UInt32 instrumentId,
                                                                 float pitch, NoteControlType type,
                                                                 float value);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOff")]
      private static extern bool BarelyInstrument_SetNoteOff(IntPtr engine, UInt32 instrumentId,
                                                             float pitch);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetNoteOn")]
      private static extern bool BarelyInstrument_SetNoteOn(IntPtr engine, UInt32 instrumentId,
                                                            float pitch);

      [DllImport(_pluginName, EntryPoint = "BarelyInstrument_SetSampleData")]
      private static extern bool BarelyInstrument_SetSampleData(IntPtr engine, UInt32 instrumentId,
                                                                [In] Slice[] slices,
                                                                Int32 sliceCount);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_CreateTask")]
      private static extern bool BarelyPerformer_CreateTask(
          IntPtr engine, UInt32 performerId, double position, double duration, Int32 priority,
          TaskEventCallback callback, ref UInt32 userData, ref UInt32 outTaskId);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Destroy")]
      private static extern bool BarelyPerformer_Destroy(IntPtr engine, UInt32 performerId);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_GetPosition")]
      private static extern bool BarelyPerformer_GetPosition(IntPtr engine, UInt32 performerId,
                                                             ref double outPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLoopBeginPosition")]
      private static extern bool BarelyPerformer_SetLoopBeginPosition(IntPtr engine,
                                                                      UInt32 performerId,
                                                                      double loopBeginPosition);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLoopLength")]
      private static extern bool BarelyPerformer_SetLoopLength(IntPtr engine, UInt32 performerId,
                                                               double loopLength);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetLooping")]
      private static extern bool BarelyPerformer_SetLooping(IntPtr engine, UInt32 performerId,
                                                            bool isLooping);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_SetPosition")]
      private static extern bool BarelyPerformer_SetPosition(IntPtr engine, UInt32 performerId,
                                                             double position);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Start")]
      private static extern bool BarelyPerformer_Start(IntPtr engine, UInt32 performerId);

      [DllImport(_pluginName, EntryPoint = "BarelyPerformer_Stop")]
      private static extern bool BarelyPerformer_Stop(IntPtr engine, UInt32 performerId);

      [DllImport(_pluginName, EntryPoint = "BarelyScale_GetPitch")]
      private static extern bool BarelyScale_GetPitch([In] ref Scale scale, Int32 degree,
                                                      ref float outPitch);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_Destroy")]
      private static extern bool BarelyTask_Destroy(IntPtr engine, UInt32 taskId);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_IsActive")]
      private static extern bool BarelyTask_IsActive(IntPtr engine, UInt32 taskId,
                                                     ref bool outIsActive);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetDuration")]
      private static extern bool BarelyTask_SetDuration(IntPtr engine, UInt32 taskId,
                                                        double duration);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetPosition")]
      private static extern bool BarelyTask_SetPosition(IntPtr engine, UInt32 taskId,
                                                        double position);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetPriority")]
      private static extern bool BarelyTask_SetPriority(IntPtr engine, UInt32 taskId,
                                                        Int32 priority);

      [DllImport(_pluginName, EntryPoint = "BarelyTask_SetEventCallback")]
      private static extern bool BarelyTask_SetEventCallback(IntPtr engine, UInt32 taskId,
                                                             TaskEventCallback callback,
                                                             ref UInt32 userData);
    }
  }
}  // namespace Barely
