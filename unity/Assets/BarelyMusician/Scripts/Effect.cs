using System;
using UnityEngine;

namespace Barely {
  /// A representation of an audio effect that can be attached to a musical instrument.
  [RequireComponent(typeof(Instrument))]
  public abstract class Effect : MonoBehaviour {
    /// Control event callback.
    ///
    /// @param index Control index.
    /// @param control Control value.
    public delegate void ControlEventCallback(int index, double value);
    public event ControlEventCallback OnControl;

    [Serializable]
    public class ControlEvent : UnityEngine.Events.UnityEvent<int, float> {}
    public ControlEvent OnControlEvent;

    /// Process order.
    public int ProcessOrder {
      get { return _processOrder; }
      set {
        if (_id == Musician.Internal.InvalidId) {
          _processOrder = value;
          return;
        }
        Musician.Internal.Effect_SetProcessOrder(_instrumentId, _id, value);
        _processOrder = Musician.Internal.Effect_GetProcessOrder(_instrumentId, _id);
      }
    }
    [SerializeField]
    private int _processOrder = 0;

    /// Returns a control value.
    ///
    /// @param index Control index.
    /// @return Control value.
    public double GetControl(int index) {
      return Musician.Internal.Effect_GetControl(_instrumentId, _id, index);
    }

    /// Resets all control values.
    public void ResetAllControls() {
      Musician.Internal.Effect_ResetAllControls(_instrumentId, _id);
    }

    /// Resets a control value.
    ///
    /// @param index Control index.
    public void ResetControl(int index) {
      Musician.Internal.Effect_ResetControl(_instrumentId, _id, index);
    }

    /// Sets a control value.
    ///
    /// @param index Control index.
    /// @param value Control value.
    /// @param slopePerBeat Control slope in value change per beat.
    public void SetControl(int index, double value, double slopePerBeat = 0.0) {
      Musician.Internal.Effect_SetControl(_instrumentId, _id, index, value, slopePerBeat);
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Effect_SetData(_instrumentId, _id, dataPtr, size);
    }

    /// Class that wraps the internal api.
    public static class Internal {
      /// Internal control event callback.
      public static void OnControlEvent(Effect effect, int index, double value) {
        effect.OnControl?.Invoke(index, value);
        effect.OnControlEvent?.Invoke(index, (float)value);
      }

      /// Re-enables effect.
      public static void ReEnable(Int64 instrumentId, Effect effect) {
        if (effect.enabled) {
          Musician.Internal.Effect_Destroy(ref effect._instrumentId, ref effect._id);
          effect._instrumentId = instrumentId;
          Musician.Internal.Effect_Create(effect._instrumentId, effect, ref effect._id);
        }
      }

      // Disables effect.
      public static void Disable(Effect effect) {
        if (effect.enabled) {
          Musician.Internal.Effect_Destroy(ref effect._instrumentId, ref effect._id);
        }
      }
    }

    protected virtual void OnEnable() {
      Instrument.Internal.SetInstrumentId(GetComponent<Instrument>(), ref _instrumentId);
      Musician.Internal.Effect_Create(_instrumentId, this, ref _id);
    }

    protected virtual void OnDisable() {
      Musician.Internal.Effect_Destroy(ref _instrumentId, ref _id);
    }

    // Instrument identifier.
    private Int64 _instrumentId = Musician.Internal.InvalidId;

    // Identifier.
    private Int64 _id = Musician.Internal.InvalidId;
  }
}
