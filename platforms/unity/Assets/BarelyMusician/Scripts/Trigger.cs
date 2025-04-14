using System;
using UnityEngine;

namespace Barely {
  /// A representation of a trigger that can performed by a musical performer in real-time.
  [Serializable]
  public class Trigger {
    /// Position in beats.
    public double Position {
      get { return _position; }
      set {
        if (_performer == null) {
          _position = value;
          return;
        }
        Engine.Internal.Trigger_SetPosition(_handle, value);
        _position = Engine.Internal.Trigger_GetPosition(_handle);
      }
    }
    [SerializeField]
    private double _position = 0.0;

    /// Process callback.
    public event Action OnProcess;

    [Serializable]
    public class ProcessEvent : UnityEngine.Events.UnityEvent {}
    public ProcessEvent OnProcessEvent;

    /// Constructs a new `Trigger`.
    ///
    /// @param position Trigger position in beats.
    /// @param callback Trigger process callback.
    /// @param onProcessEvent Trigger process event.
    public Trigger(double position, Action callback, ProcessEvent onProcessEvent = null) {
      _position = position;
      OnProcess = callback;
      OnProcessEvent = onProcessEvent;
    }

    ~Trigger() {
      Engine.Internal.Trigger_Destroy(Performer.Internal.GetHandle(_performer), ref _handle);
    }

    /// Updates the trigger.
    ///
    /// @param performer Performer.
    public void Update(Performer performer) {
      if (_performer == performer && _handle != IntPtr.Zero) {
        Position = _position;
        return;
      }
      Engine.Internal.Trigger_Destroy(Performer.Internal.GetHandle(_performer), ref _handle);
      _performer = performer;
      if (_performer != null) {
        Engine.Internal.Trigger_Create(this, Performer.Internal.GetHandle(_performer), _position,
                                       ref _handle);
      }
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Internal process callback.
      public static void OnProcess(Trigger trigger) {
        trigger.OnProcess?.Invoke();
        trigger.OnProcessEvent?.Invoke();
      }
    }

    /// Performer.
    private Performer _performer = null;

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
