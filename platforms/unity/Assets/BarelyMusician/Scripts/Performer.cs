using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// A representation of a musical performer that can perform rhytmic tasks in real-time.
  public class Performer : MonoBehaviour {
    /// True if playing on awake, false otherwise.
    public bool PlayOnAwake = false;
    private bool _playOnEnable = false;

    /// True if looping, false otherwise.
    public bool Loop {
      get { return _loop; }
      set {
        if (_handle == IntPtr.Zero) {
          _loop = value;
          return;
        }
        Engine.Internal.Performer_SetLooping(_handle, value);
        _loop = Engine.Internal.Performer_IsLooping(_handle);
      }
    }
    [SerializeField]
    private bool _loop = false;

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        if (_handle == IntPtr.Zero) {
          _loopBeginPosition = value;
          return;
        }
        Engine.Internal.Performer_SetLoopBeginPosition(_handle, value);
        _loopBeginPosition = Engine.Internal.Performer_GetLoopBeginPosition(_handle);
      }
    }
    [SerializeField]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        if (_handle == IntPtr.Zero) {
          _loopLength = value;
          return;
        }
        Engine.Internal.Performer_SetLoopLength(_handle, value);
        _loopLength = Engine.Internal.Performer_GetLoopLength(_handle);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// List of recurring tasks.
    public List<Task> Tasks = new List<Task>();

    /// Beat callback.
    public delegate void BeatCallback();
    public event BeatCallback OnBeat;

    [Serializable]
    public class BeatEvent : UnityEngine.Events.UnityEvent {}
    public BeatEvent OnBeatEvent;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Engine.Internal.Performer_IsPlaying(_handle); }
    }

    /// Position in beats.
    public double Position {
      get { return Engine.Internal.Performer_GetPosition(_handle); }
      set { Engine.Internal.Performer_SetPosition(_handle, value); }
    }

    /// Starts the performer.
    public void Play() {
      _playOnEnable = (_handle == IntPtr.Zero);
      Engine.Internal.Performer_Start(_handle);
    }

    /// Stops the performer.
    public void Stop() {
      _playOnEnable = false;
      Engine.Internal.Performer_Stop(_handle);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the handle.
      public static IntPtr GetHandle(Performer performer) {
        return performer ? performer._handle : IntPtr.Zero;
      }

      /// Internal beat callback.
      public static void OnBeat(Performer performer) {
        performer.OnBeat?.Invoke();
        performer.OnBeatEvent?.Invoke();
      }
    }

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;

    private void OnEnable() {
      Engine.Internal.Performer_Create(this, ref _handle);
      Update();
      if (PlayOnAwake || _playOnEnable) {
        Play();
      }
    }

    private void Start() {
      if (PlayOnAwake && !IsPlaying) {
        Play();
      }
    }

    private void OnDisable() {
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(null);
      }
      Engine.Internal.Performer_Destroy(ref _handle);
    }

    private void Update() {
      Loop = _loop;
      LoopBeginPosition = _loopBeginPosition;
      LoopLength = _loopLength;
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(this);
      }
    }
  }
}  // namespace Barely
