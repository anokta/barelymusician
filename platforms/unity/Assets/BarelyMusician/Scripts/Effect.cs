using System;
using UnityEngine;

namespace Barely {
  /// A representation of an audio effect that can be attached to a musical instrument.
  [RequireComponent(typeof(Instrument))]
  public abstract class Effect : MonoBehaviour {
    /// Control event callback.
    ///
    /// @param id Control identifier.
    /// @param control Control value.
    public delegate void ControlEventCallback(int id, double value);
    public event ControlEventCallback OnControl;

    [Serializable]
    public class ControlEvent : UnityEngine.Events.UnityEvent<int, float> {}
    public ControlEvent OnControlEvent;

    /// Process order.
    public int ProcessOrder {
      get { return _processOrder; }
      set {
        if (_handle == IntPtr.Zero) {
          _processOrder = value;
          return;
        }
        Musician.Internal.Effect_SetProcessOrder(_handle, value);
        _processOrder = Musician.Internal.Effect_GetProcessOrder(_handle);
      }
    }
    [SerializeField]
    private int _processOrder = 0;

    /// Returns a control value.
    ///
    /// @param id Control identifier.
    /// @return Control value.
    public double GetControl(int id) {
      return Musician.Internal.Effect_GetControl(_handle, id);
    }

    /// Resets all control values.
    public void ResetAllControls() {
      Musician.Internal.Effect_ResetAllControls(_handle);
    }

    /// Resets a control value.
    ///
    /// @param id Control identifier.
    public void ResetControl(int id) {
      Musician.Internal.Effect_ResetControl(_handle, id);
    }

    /// Sets a control value.
    ///
    /// @param id Control identifier.
    /// @param value Control value.
    public void SetControl(int id, double value) {
      Musician.Internal.Effect_SetControl(_handle, id, value);
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Effect_SetData(_handle, dataPtr, size);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Internal control event callback.
      public static void OnControlEvent(Effect effect, int id, double value) {
        effect.OnControl?.Invoke(id, value);
        effect.OnControlEvent?.Invoke(id, (float)value);
      }
    }

    protected virtual void OnEnable() {
      _instrument = GetComponent<Instrument>();
      _instrument.OnInstrumentCreate += OnInstrumentCreate;
      _instrument.OnInstrumentDestroy += OnInstrumentDestroy;
      Musician.Internal.Effect_Create(_instrument, this, ref _handle);
    }

    protected virtual void OnDisable() {
      Musician.Internal.Effect_Destroy(ref _handle);
      _instrument.OnInstrumentCreate -= OnInstrumentCreate;
      _instrument.OnInstrumentDestroy -= OnInstrumentDestroy;
      _instrument = null;
    }

    private void OnInstrumentCreate() {
      Musician.Internal.Effect_Create(_instrument, this, ref _handle);
    }

    private void OnInstrumentDestroy() {
      Musician.Internal.Effect_Destroy(ref _handle);
    }

    // Handle.
    private IntPtr _handle = IntPtr.Zero;

    // Instrument.
    private Instrument _instrument = null;
  }
}  // namespace Barely
