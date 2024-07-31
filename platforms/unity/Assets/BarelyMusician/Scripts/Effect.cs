using System;
using UnityEngine;

namespace Barely {
  /// A representation of an audio effect that can be attached to an audio source.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Effect : MonoBehaviour {
    /// Returns a control value.
    ///
    /// @param controlId Control identifier.
    /// @return Control value.
    public double GetControl(int controlId) {
      return Musician.Internal.Effect_GetControl(_ptr, controlId);
    }

    /// Resets a control value.
    ///
    /// @param controlId Control identifier.
    public void ResetControl(int controlId) {
      Musician.Internal.Effect_ResetControl(_ptr, controlId);
    }

    /// Sets a control value.
    ///
    /// @param controlId Control identifier.
    /// @param value Control value.
    public void SetControl(int controlId, double value) {
      Musician.Internal.Effect_SetControl(_ptr, controlId, value);
    }

    /// Sets data.
    ///
    /// @param dataPtr Pointer to data.
    /// @param size Data size in bytes.
    public void SetData(IntPtr dataPtr, int size) {
      Musician.Internal.Effect_SetData(_ptr, dataPtr, size);
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
