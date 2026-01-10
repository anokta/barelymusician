using System;
using UnityEngine;

namespace Barely {
  /// Task event types.
  public enum TaskEventType {
    /// Begin.
    [InspectorName("Begin")] BEGIN = 0,
    /// End.
    [InspectorName("End")] END,
    /// Update.
    [InspectorName("Update")] UPDATE,
  }

  /// A representation of a recurring task that can performed by a musical performer in real-time.
  [Serializable]
  public class Task {
    /// Position in beats.
    public double Position {
      get { return _position; }
      set {
        if (_performer == null) {
          _position = value;
          return;
        }
        Engine.Internal.Task_SetPosition(_id, value);
        _position = Engine.Internal.Task_GetPosition(_id);
      }
    }
    [SerializeField]
    private double _position = 0.0;

    /// Duration in beats.
    public double Duration {
      get { return _duration; }
      set {
        if (_performer == null) {
          _duration = value;
          return;
        }
        Engine.Internal.Task_SetDuration(_id, value);
        _duration = Engine.Internal.Task_GetDuration(_id);
      }
    }
    [SerializeField]
    [Min(0.0f)]
    private double _duration = 0.0;

    /// Priority.
    public int Priority {
      get { return _priority; }
      set {
        if (_performer == null) {
          _priority = value;
          return;
        }
        Engine.Internal.Task_SetPriority(_id, value);
        _priority = Engine.Internal.Task_GetPriority(_id);
      }
    }
    [SerializeField]
    private int _priority = 0;

    /// Process callback.
    public delegate void ProcessCallback(TaskEventType type);
    public event ProcessCallback OnProcess;

    [Serializable]
    public class ProcessEvent : UnityEngine.Events.UnityEvent<TaskEventType> {}
    public ProcessEvent OnProcessEvent;

    /// Constructs a new `Task`.
    ///
    /// @param position Task position in beats.
    /// @param duration Task duration in beats.
    /// @param priority Task priority.
    /// @param callback Task process callback.
    /// @param onProcessEvent Task process event.
    public Task(double position, double duration, int priority, ProcessCallback callback,
                ProcessEvent onProcessEvent = null) {
      _position = position;
      _duration = duration;
      _priority = priority;
      OnProcess = callback;
      OnProcessEvent = onProcessEvent;
    }

    ~Task() {
      Engine.Internal.Task_Destroy(Performer.Internal.GetId(_performer), ref _id);
    }

    /// Updates the task.
    ///
    /// @param performer Performer.
    public void Update(Performer performer) {
      if (_performer == performer && _id > 0) {
        Position = _position;
        Duration = _duration;
        Priority = _priority;
        return;
      }
      Engine.Internal.Task_Destroy(Performer.Internal.GetId(_performer), ref _id);
      _performer = performer;
      if (_performer != null) {
        Engine.Internal.Task_Create(this, Performer.Internal.GetId(_performer), _position,
                                    _duration, _priority, ref _id);
      }
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Internal process callback.
      public static void OnProcess(Task task, TaskEventType type) {
        task.OnProcess?.Invoke(type);
        task.OnProcessEvent?.Invoke(type);
      }
    }

    /// Performer.
    private Performer _performer = null;

    // Identifier.
    private UInt32 _id = 0;
  }
}  // namespace Barely
