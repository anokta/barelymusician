using System;
using UnityEngine;

namespace Barely {
  /// A representation of a recurring task that can performed by a musical performer in real-time.
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
    }

    ~Task() {
      Musician.Internal.Task_Destroy(Performer.Internal.GetHandle(_performer), ref _handle);
    }

    /// Updates the task.
    ///
    /// @param performer Performer.
    public void Update(Performer performer) {
      if (_performer == performer && _handle != IntPtr.Zero) {
        Position = _position;
        return;
      }
      Musician.Internal.Task_Destroy(Performer.Internal.GetHandle(_performer), ref _handle);
      _performer = performer;
      if (_performer != null) {
        Musician.Internal.Task_Create(Performer.Internal.GetHandle(_performer), delegate() {
          OnProcess?.Invoke();
          OnProcessEvent?.Invoke();
        }, _position, ref _handle);
      }
    }

    /// Performer.
    private Performer _performer = null;

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
