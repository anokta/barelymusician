using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// Performer.
  public class Performer : MonoBehaviour {
    /// Recurring task.
    [Serializable]
    public class Task {
      /// Performer identifier.
      public Int64 PerformerId { get; private set; } = Musician.Internal.InvalidId;

      /// Identifier.
      public Int64 Id { get; private set; } = Musician.Internal.InvalidId;

      /// Process callback.
      public Action OnProcess;
      public UnityEngine.Events.UnityEvent OnProcessEvent;

      /// Position in beats.
      public double Position {
        get { return _position; }
        set {
          Musician.Internal.Performer_SetTaskPosition(this, value);
          _position = Musician.Internal.Performer_GetTaskPosition(this);
        }
      }
      [SerializeField]
      private double _position = 0.0;

      /// Process order.
      public int ProcessOrder {
        get { return _processOrder; }
        set {
          Musician.Internal.Performer_SetTaskProcessOrder(this, value);
          _processOrder = Musician.Internal.Performer_GetTaskProcessOrder(this);
        }
      }
      [SerializeField]
      private int _processOrder = 0;

      /// Constructs a new `Task`.
      ///
      /// @param callback Task process callback.
      /// @param position Task position in beats.
      /// @param processOrder Task process order.
      public Task(Action callback, double position, int processOrder = 0) {
        OnProcess = callback;
        _position = position;
        _processOrder = processOrder;
      }

      /// Updates the task.
      ///
      /// @param performer Performer.
      public void Update(Performer performer) {
        if (PerformerId == Musician.Internal.InvalidId) {
          PerformerId = performer.Id;
          _definition = new Musician.Internal.TaskDefinition();
          _definition.createCallback = delegate() {
            performer._tasks.Add(this);
          };
          _definition.destroyCallback = delegate() {
            performer._tasks.Remove(this);
            PerformerId = Musician.Internal.InvalidId;
            Id = Musician.Internal.InvalidId;
          };
          _definition.processCallback = delegate() {
            OnProcess?.Invoke();
            OnProcessEvent?.Invoke();
          };
          Id = Musician.Internal.Performer_CreateTask(performer, _definition, /*isOneOff=*/false,
                                                      _position, _processOrder);
        } else {
          Position = _position;
          ProcessOrder = _processOrder;
        }
      }

      // Task definition.
      private Musician.Internal.TaskDefinition _definition;
    }

    /// Identifier.
    public Int64 Id { get; private set; } = Musician.Internal.InvalidId;

    /// True if playing on awake, false otherwise.
    public bool playOnAwake = false;

    /// True if looping, false otherwise.
    public bool Loop {
      get { return _loop; }
      set {
        Musician.Internal.Performer_SetLooping(this, value);
        _loop = Musician.Internal.Performer_IsLooping(this);
      }
    }
    [SerializeField]
    private bool _loop = false;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Performer_IsPlaying(this); }
    }

    /// Loop begin position in beats.
    public double LoopBeginPosition {
      get { return _loopBeginPosition; }
      set {
        Musician.Internal.Performer_SetLoopBeginPosition(this, value);
        _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(this);
      }
    }
    [SerializeField]
    private double _loopBeginPosition = 0.0;

    /// Loop length in beats.
    public double LoopLength {
      get { return _loopLength; }
      set {
        Musician.Internal.Performer_SetLoopLength(this, value);
        _loopLength = Musician.Internal.Performer_GetLoopLength(this);
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

    /// List of recurring tasks.
    public List<Task> Tasks = new List<Task>();
    private List<Task> _tasks = null;

    /// Starts the performer.
    public void Play() {
      Musician.Internal.Performer_Start(this);
    }

    /// Schedules a one-off task at a specific time.
    ///
    /// @param callback Task process callback.
    /// @param position Task position in beats.
    /// @param processOrder Task process order.
    public void ScheduleTask(Action callback, double position, int processOrder = 0) {
      var definition = new Musician.Internal.TaskDefinition();
      definition.createCallback = delegate() {
        _oneOffDefinitions.Add(definition);
      };
      definition.destroyCallback = delegate() {
        _oneOffDefinitions.Remove(definition);
      };
      definition.processCallback = callback;
      Musician.Internal.Performer_CreateTask(this, definition, /*isOneOff=*/true, position,
                                             processOrder);
    }

    /// Stops the performer.
    public void Stop() {
      Musician.Internal.Performer_Stop(this);
    }

    // List of one-off task definitions.
    private List<Musician.Internal.TaskDefinition> _oneOffDefinitions = null;

    private void Awake() {
      _oneOffDefinitions = new List<Musician.Internal.TaskDefinition>();
      _tasks = new List<Task>();
    }

    private void OnDestroy() {
      _oneOffDefinitions = null;
      _tasks = null;
    }

    private void OnEnable() {
      Id = Musician.Internal.Performer_Create(this);
      Musician.Internal.Performer_SetLooping(this, _loop);
      _loop = Musician.Internal.Performer_IsLooping(this);
      Musician.Internal.Performer_SetLoopBeginPosition(this, _loopBeginPosition);
      _loopBeginPosition = Musician.Internal.Performer_GetLoopBeginPosition(this);
      Musician.Internal.Performer_SetLoopLength(this, _loopLength);
      _loopLength = Musician.Internal.Performer_GetLoopLength(this);
      for (int i = 0; i < Tasks.Count; ++i) {
        Tasks[i].Update(this);
      }
    }

    private void OnDisable() {
      Musician.Internal.Performer_Destroy(this);
      Id = Musician.Internal.InvalidId;
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
      if (Tasks.Count < _tasks.Count) {
        for (int i = 0; i < _tasks.Count; ++i) {
          if (Tasks.IndexOf(_tasks[i]) == -1) {
            Musician.Internal.Performer_DestroyTask(_tasks[i--]);
          }
        }
      }
    }
  }
}  // namespace Barely
