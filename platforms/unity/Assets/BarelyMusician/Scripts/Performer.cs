using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// A representation of a musical performer that can perform rhytmic tasks in real-time.
  public class Performer : MonoBehaviour {
    /// True if playing on awake, false otherwise.
    public bool PlayOnAwake = false;

    /// True if looping, false otherwise.
    public bool Loop {
      get { return _loop; }
      set {
        if (_ptr == IntPtr.Zero) {
          _loop = value;
          return;
        }
        Musician.Internal.Performer_SetLooping(_ptr, value);
        _loop = Musician.Internal.Performer_IsLooping(_ptr);
      }
    }
    [SerializeField]
    private bool _loop = false;

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        if (_ptr == IntPtr.Zero) {
          _loopBeginPosition = value;
          return;
        }
        Musician.Internal.Performer_SetLoopBeginPosition(_ptr, value);
        _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(_ptr);
      }
    }
    [SerializeField]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        if (_ptr == IntPtr.Zero) {
          _loopLength = value;
          return;
        }
        Musician.Internal.Performer_SetLoopLength(_ptr, value);
        _loopLength = Musician.Internal.Performer_GetLoopLength(_ptr);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// Position in beats.
    public double Position {
      get { return Musician.Internal.Performer_GetPosition(_ptr); }
      set { Musician.Internal.Performer_SetPosition(_ptr, value); }
    }

    /// List of recurring tasks.
    public List<Task> Tasks = new List<Task>();

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Performer_IsPlaying(_ptr); }
    }

    /// Starts the performer.
    public void Play() {
      Musician.Internal.Performer_Start(_ptr);
    }

    /// Cancels all one-off tasks.
    public void CancelAllOneOffTasks() {
      Musician.Internal.Performer_CancelAllOneOffTasks(_ptr);
    }

    /// Schedules a one-off task at a specific position.
    ///
    /// @param callback Task process callback.
    /// @param position Task position in beats.
    /// @param processOrder Task process order.
    public void ScheduleOneOffTask(Action callback, double position) {
      Musician.Internal.Performer_ScheduleOneOffTask(_ptr, callback, position);
    }

    /// Stops the performer.
    public void Stop() {
      Musician.Internal.Performer_Stop(_ptr);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Returns the pointer.
      public static IntPtr GetPtr(Performer performer) {
        return performer ? performer._ptr : IntPtr.Zero;
      }
    }

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;

    private void OnEnable() {
      Musician.Internal.Performer_Create(this, ref _ptr);
      Loop = _loop;
      LoopBeginPosition = _loopBeginPosition;
      LoopLength = _loopLength;
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(this);
      }
    }

    private void OnDisable() {
      Musician.Internal.Performer_Destroy(ref _ptr);
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(null);
      }
    }

    void Start() {
      if (PlayOnAwake) {
        Play();
      }
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
