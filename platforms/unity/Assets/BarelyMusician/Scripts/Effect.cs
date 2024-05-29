using System;
using UnityEngine;

namespace Barely {
  /// A representation of an audio effect that can be attached to an audio source.
  [RequireComponent(typeof(AudioSource))]
  public abstract class Effect : MonoBehaviour {
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

    protected virtual void OnEnable() {
      Musician.Internal.Effect_Create(this, ref _handle);
    }

    protected virtual void OnDisable() {
      Musician.Internal.Effect_Destroy(ref _handle);
    }

    private void OnAudioFilterRead(float[] data, int channels) {
      Musician.Internal.Effect_Process(_handle, data, channels);
    }

    // Handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
