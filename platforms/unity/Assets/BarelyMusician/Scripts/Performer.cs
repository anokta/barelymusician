using System;
using System.Collections.Generic;
using UnityEngine;

namespace Barely {
  /// Performer.
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
          Musician.Internal.Performer_SetTaskPosition(_performerId, _id, value);
          _position = Musician.Internal.Performer_GetTaskPosition(_performerId, _id);
        }
      }
      [SerializeField]
      private double _position = 0.0;

      /// Process order.
      public int ProcessOrder {
        get { return _processOrder; }
        set {
          Musician.Internal.Performer_SetTaskProcessOrder(_performerId, _id, value);
          _processOrder = Musician.Internal.Performer_GetTaskProcessOrder(_performerId, _id);
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
        if (_performerId == Musician.Internal.InvalidId) {
          _performerId = performer._id;
          _definition = new Musician.Internal.TaskDefinition();
          _definition.createCallback = delegate() {
            performer._tasks.Add(this);
          };
          _definition.destroyCallback = delegate() {
            performer._tasks.Remove(this);
            _performerId = Musician.Internal.InvalidId;
            _id = Musician.Internal.InvalidId;
          };
          _definition.processCallback = delegate() {
            OnProcess?.Invoke();
            OnProcessEvent?.Invoke();
          };
          _id = Musician.Internal.Performer_CreateTask(
              performer._id, _definition, /*isOneOff=*/false, _position, _processOrder);
        } else {
          if (performer.Tasks.Count < performer._tasks.Count &&
              performer.Tasks.IndexOf(this) == -1) {
            Musician.Internal.Performer_DestroyTask(_performerId, _id);
          } else {
            Position = _position;
            ProcessOrder = _processOrder;
          }
        }
      }

      // Task definition.
      private Musician.Internal.TaskDefinition _definition;

      // Identifier.
      private Int64 _id = Musician.Internal.InvalidId;

      /// Performer identifier.
      public Int64 _performerId = Musician.Internal.InvalidId;
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
    private List<Task> _tasks = null;

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
      var definition = new Musician.Internal.TaskDefinition();
      definition.createCallback = delegate() {
        _oneOffDefinitions.Add(definition);
      };
      definition.destroyCallback = delegate() {
        _oneOffDefinitions.Remove(definition);
      };
      definition.processCallback = callback;
      Musician.Internal.Performer_CreateTask(_id, definition, /*isOneOff=*/true, position,
                                             processOrder);
    }

    /// Stops the performer.
    public void Stop() {
      Musician.Internal.Performer_Stop(_id);
    }

    // Identifier.
    private Int64 _id = Musician.Internal.InvalidId;

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
      _id = Musician.Internal.Performer_Create(this);
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
      Musician.Internal.Performer_Destroy(_id);
      _id = Musician.Internal.InvalidId;
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
    }
  }
}  // namespace Barely
