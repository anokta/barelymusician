using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// A representation of a musical performer that can perform rhytmic tasks in real-time.
  public class Performer : MonoBehaviour {
    /// Recurring task.
    [Serializable]
    public class Task {
      /// Process callback.
      public Action OnProcess;
      public UnityEngine.Events.UnityEvent OnProcessEvent;

      /// Position in beats.
      public double Position {
        get { return _position; }
        set {
          if (_performer == null) {
            _position = value;
            return;
          }
          Musician.Internal.Task_SetPosition(_handle, value);
          _position = Musician.Internal.Task_GetPosition(_handle);
        }
      }
      [SerializeField]
      private double _position = 0.0;

      /// Process order.
      public int ProcessOrder {
        get { return _processOrder; }
        set {
          if (_performer == null) {
            _processOrder = value;
            return;
          }
          Musician.Internal.Task_SetProcessOrder(_handle, value);
          _processOrder = Musician.Internal.Task_GetProcessOrder(_handle);
        }
      }
      [SerializeField]
      private int _processOrder = 0;

      /// Constructs a new `Task`.
      ///
      /// @param callback Task process callback.
      /// @param position Task position in beats.
      /// @param processOrder Task process order.
      /// @param onProcessEvent Task process event.
      public Task(Action callback, double position, int processOrder = 0,
                  UnityEngine.Events.UnityEvent onProcessEvent = null) {
        OnProcess = callback;
        OnProcessEvent = onProcessEvent;
        _position = position;
        _processOrder = processOrder;
      }

      /// Updates the task.
      ///
      /// @param performer Performer.
      public void Update(Performer performer) {
        if (_performer == null) {
          _performer = performer;
          Musician.Internal.Task_Create(_performer._handle, delegate() {
            OnProcess?.Invoke();
            OnProcessEvent?.Invoke();
          }, _position, _processOrder, ref _handle);
          _performer._tasks.Add(this);
        } else if (performer == null) {
          Musician.Internal.Task_Destroy(ref _handle);
          _performer._tasks.Remove(this);
          _performer = null;
        } else {
          Position = _position;
          ProcessOrder = _processOrder;
        }
      }

      // Handle.
      private IntPtr _handle = IntPtr.Zero;

      /// Performer.
      private Performer _performer = null;
    }

    /// True if playing on awake, false otherwise.
    public bool playOnAwake = false;

    /// True if looping, false otherwise.
    public bool Loop {
      get { return _loop; }
      set {
        Musician.Internal.Performer_SetLooping(_handle, value);
        _loop = Musician.Internal.Performer_IsLooping(_handle);
      }
    }
    [SerializeField]
    private bool _loop = false;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Performer_IsPlaying(_handle); }
    }

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        Musician.Internal.Performer_SetLoopBeginPosition(_handle, value);
        _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(_handle);
      }
    }
    [SerializeField]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        Musician.Internal.Performer_SetLoopLength(_handle, value);
        _loopLength = Musician.Internal.Performer_GetLoopLength(_handle);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// Position in beats.
    public double Position {
      get { return Musician.Internal.Performer_GetPosition(_handle); }
      set { Musician.Internal.Performer_SetPosition(_handle, value); }
    }

    /// List of recurring tasks.
    public List<Task> Tasks = new List<Task>();
    private List<Task> _tasks = new List<Task>();

    /// Starts the performer.
    public void Play() {
      Musician.Internal.Performer_Start(_handle);
    }

    /// Schedules a one-off task at a specific position.
    ///
    /// @param callback Task process callback.
    /// @param position Task position in beats.
    /// @param processOrder Task process order.
    public void ScheduleOneOffTask(Action callback, double position, int processOrder = 0) {
      Musician.Internal.Performer_ScheduleOneOffTask(_handle, callback, position, processOrder);
    }

    /// Stops the performer.
    public void Stop() {
      Musician.Internal.Performer_Stop(_handle);
    }

    // Handle.
    private IntPtr _handle = IntPtr.Zero;

    private void OnEnable() {
      Musician.Internal.Performer_Create(this, ref _handle);
      Musician.Internal.Performer_SetLooping(_handle, _loop);
      _loop = Musician.Internal.Performer_IsLooping(_handle);
      Musician.Internal.Performer_SetLoopBeginPosition(_handle, _loopBeginPosition);
      _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(_handle);
      Musician.Internal.Performer_SetLoopLength(_handle, _loopLength);
      _loopLength = Musician.Internal.Performer_GetLoopLength(_handle);
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(this);
      }
    }

    private void OnDisable() {
      Musician.Internal.Performer_Destroy(ref _handle);
      _tasks.Clear();
    }

    void Start() {
      if (playOnAwake) {
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
      for (int i = 0; i < _tasks.Count; ++i) {
        if (Tasks.IndexOf(_tasks[i]) == -1) {
          _tasks[i].Update(null);
        }
      }
    }
  }
}  // namespace Barely
