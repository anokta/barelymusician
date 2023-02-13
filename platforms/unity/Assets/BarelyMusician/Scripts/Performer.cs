using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// Performer.
  public class Performer : MonoBehaviour {
    /// Task type enum.
    public enum TaskType {
      /// One-off task.
      [InspectorName("One-off")] ONE_OFF = 0,
      /// Recurring task.
      [InspectorName("Recurring")] RECURRING = 1,
    }

    /// Task.
    public class Task {
      /// Performer.
      public Performer Performer { get; private set; } = null;

      /// Identifier.
      public Int64 Id { get; private set; } = Musician.Internal.InvalidId;

      /// Type.
      public TaskType Type { get; private set; } = TaskType.RECURRING;

      /// Action.
      public Action Action { get; set; }
      private Action _processCallback = null;

      /// Position in beats.
      public double Position {
        get { return _position; }
        set {
          if (_position != value) {
            Musician.Internal.Performer_SetTaskPosition(this, value);
            _position = Musician.Internal.Performer_GetTaskPosition(this);
          }
        }
      }
      private double _position = 0.0;

      /// Process order.
      public int ProcessOrder {
        get { return _processOrder; }
        set {
          if (_processOrder != value) {
            Musician.Internal.Performer_SetTaskProcessOrder(this, value);
            _processOrder = Musician.Internal.Performer_GetTaskProcessOrder(this);
          }
        }
      }
      private int _processOrder = 0;

      public Task(Performer performer, TaskType type, Action action, double position,
                  int processOrder) {
        Performer = performer;
        Type = type;
        Action = action;
        _position = position;
        _processOrder = processOrder;
        _processCallback = delegate() {
          Action?.Invoke();
        };
        Initialize();
      }

      public void Initialize() {
        Id = Musician.Internal.Performer_CreateTask(Performer, Type, _processCallback, _position,
                                                    _processOrder);
      }
    }

    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Internal.InvalidId;

    /// True if looping, false otherwise.
    public bool IsLooping {
      get { return _isLooping; }
      set {
        if (_isLooping != value) {
          Musician.Internal.Performer_SetLooping(this, value);
          _isLooping = Musician.Internal.Performer_IsLooping(this);
        }
      }
    }
    [SerializeField]
    private bool _isLooping = false;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Performer_IsPlaying(this); }
    }

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        if (_loopBeginPosition != value) {
          Musician.Internal.Performer_SetLoopBeginPosition(this, value);
          _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(this);
        }
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        if (_loopLength != value) {
          Musician.Internal.Performer_SetLoopLength(this, value);
          _loopLength = Musician.Internal.Performer_GetLoopLength(this);
        }
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _loopLength = 1.0;

    /// Position in beats.
    public double Position {
      get { return Musician.Internal.Performer_GetPosition(this); }
      set { Musician.Internal.Performer_SetPosition(this, value); }
    }

    /// Creates a new task.
    ///
    /// @param type Task type.
    /// @param action Task action.
    /// @param position Task position in beats.
    /// @param processOrder Task process order.
    /// @return Task.
    public Task CreateTask(TaskType type, Action action, double position = 0.0,
                           int processOrder = 0) {
      Task task = new Task(this, type, action, position, processOrder);
      _tasks.Add(task.Id, task);
      return task;
    }

    /// Destroys a task.
    ///
    /// @param task Task.
    public void DestroyTask(Task task) {
      if (_tasks.Remove(task.Id)) {
        Musician.Internal.Performer_DestroyTask(task);
      }
    }

    /// Starts the performer.
    public void Play() {
      Musician.Internal.Performer_Start(this);
    }

    /// Stops the performer.
    public void Stop() {
      Musician.Internal.Performer_Stop(this);
    }

    // Tasks.
    private Dictionary<Int64, Task> _tasks = null;

    private void Awake() {
      _tasks = new Dictionary<Int64, Task>();
    }

    private void OnDestroy() {
      _tasks = null;
    }

    private void OnEnable() {
      Id = Musician.Internal.Performer_Create(this);
      Musician.Internal.Performer_SetLooping(this, _isLooping);
      _isLooping = Musician.Internal.Performer_IsLooping(this);
      Musician.Internal.Performer_SetLoopBeginPosition(this, _loopBeginPosition);
      _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(this);
      Musician.Internal.Performer_SetLoopLength(this, _loopLength);
      _loopLength = Musician.Internal.Performer_GetLoopLength(this);
      foreach (var task in _tasks.Values) {
        task.Initialize();
      }
    }

    private void OnDisable() {
      Musician.Internal.Performer_Destroy(this);
      Id = Musician.Internal.InvalidId;
    }

    private void OnValidate() {
      IsLooping = _isLooping;
      LoopBeginPosition = _loopBeginPosition;
      LoopLength = _loopLength;
    }
  }
}  // namespace Barely
