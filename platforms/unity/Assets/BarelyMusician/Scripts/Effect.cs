using System;
using UnityEngine;

namespace Barely {
  /// A representation of an audio effect that can be attached to an audio source.
  [RequireComponent(typeof(AudioSource))]
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

    /// Returns a control value.
    ///
    /// @param id Control identifier.
    /// @return Control value.
    public double GetControl(int id) {
      return Musician.Internal.Effect_GetControl(_ptr, id);
    }

    /// Resets a control value.
    ///
    /// @param id Control identifier.
    public void ResetControl(int id) {
      Musician.Internal.Effect_ResetControl(_ptr, id);
    }

    /// Sets a control value.
    ///
    /// @param id Control identifier.
    /// @param value Control value.
    public void SetControl(int id, double value) {
      Musician.Internal.Effect_SetControl(_ptr, id, value);
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Effect_SetData(_ptr, dataPtr, size);
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
      Musician.Internal.Effect_Create(this, ref _ptr);
    }

    protected virtual void OnDisable() {
      Musician.Internal.Effect_Destroy(ref _ptr);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Effect_Process(_ptr, data, channels);
    }

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
