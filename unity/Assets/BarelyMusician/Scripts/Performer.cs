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
          Musician.Internal.Task_SetPosition(_performer._id, _id, value);
          _position = Musician.Internal.Task_GetPosition(_performer._id, _id);
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
          Musician.Internal.Task_SetProcessOrder(_performer._id, _id, value);
          _processOrder = Musician.Internal.Task_GetProcessOrder(_performer._id, _id);
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
          Musician.Internal.Task_Create(_performer._id,
                                        delegate() {
                                          OnProcess?.Invoke();
                                          OnProcessEvent?.Invoke();
                                        },
                                        /*isOneOff=*/false, _position, _processOrder, ref _id);
          _performer._tasks.Add(this);
        } else if (performer == null) {
          Musician.Internal.Task_Destroy(_performer._id, ref _id);
          _performer._tasks.Remove(this);
          _performer = null;
        } else {
          Position = _position;
          ProcessOrder = _processOrder;
        }
      }

      // Task identifier.
      private Int64 _id = Musician.Internal.InvalidId;

      /// Performer.
      private Performer _performer = null;
    }

    /// True if playing on awake, false otherwise.
    public bool playOnAwake = false;

    /// True if looping, false otherwise.
    public bool Loop {
      get { return _loop; }
      set {
        Musician.Internal.Performer_SetLooping(_id, value);
        _loop = Musician.Internal.Performer_IsLooping(_id);
      }
    }
    [SerializeField]
    private bool _loop = false;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Performer_IsPlaying(_id); }
    }

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        Musician.Internal.Performer_SetLoopBeginPosition(_id, value);
        _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(_id);
      }
    }
    [SerializeField]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        Musician.Internal.Performer_SetLoopLength(_id, value);
        _loopLength = Musician.Internal.Performer_GetLoopLength(_id);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// Position in beats.
    public double Position {
      get { return Musician.Internal.Performer_GetPosition(_id); }
      set { Musician.Internal.Performer_SetPosition(_id, value); }
    }

    /// List of recurring tasks.
    public List<Task> Tasks = new List<Task>();
    private List<Task> _tasks = new List<Task>();

    /// Starts the performer.
    public void Play() {
      Musician.Internal.Performer_Start(_id);
    }

    /// Schedules a one-off task at a specific time.
    ///
    /// @param callback Task process callback.
    /// @param position Task position in beats.
    /// @param processOrder Task process order.
    public void ScheduleTask(Action callback, double position, int processOrder = 0) {
      Int64 taskId = Musician.Internal.InvalidId;
      Musician.Internal.Task_Create(_id, callback, /*isOneOff=*/true, position, processOrder,
                                    ref taskId);
    }

    /// Stops the performer.
    public void Stop() {
      Musician.Internal.Performer_Stop(_id);
    }

    // Identifier.
    private Int64 _id = Musician.Internal.InvalidId;

    private void OnEnable() {
      Musician.Internal.Performer_Create(this, ref _id);
      Musician.Internal.Performer_SetLooping(_id, _loop);
      _loop = Musician.Internal.Performer_IsLooping(_id);
      Musician.Internal.Performer_SetLoopBeginPosition(_id, _loopBeginPosition);
      _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(_id);
      Musician.Internal.Performer_SetLoopLength(_id, _loopLength);
      _loopLength = Musician.Internal.Performer_GetLoopLength(_id);
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(this);
      }
    }

    private void OnDisable() {
      Musician.Internal.Performer_Destroy(ref _id);
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
