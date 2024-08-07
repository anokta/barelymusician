using System;
using System.Runtime.InteropServices;
using Barely;
using UnityEditor.PackageManager.Requests;

namespace Barely {
  public class Control {
    /// Control definition.
    [StructLayout(LayoutKind.Sequential)]
    public struct Definition {
      /// Identifier.
      public Int32 controlId;

      /// Default value.
      public double defaultValue;

      /// Minimum value.
      public double minValue;

      /// Maximum value.
      public double maxValue;
    }

    /// Value.
    public double Value {
      get { return Musician.Internal.Control_GetValue(_ptr); }
      set { Musician.Internal.Control_SetValue(_ptr, value); }
    }

    /// Constructs a new `Control`.
    ///
    /// @param ptr Pointer to control.
    public Control(IntPtr ptr) {
      _ptr = ptr;
    }

    /// Resets the value.
    public void Reset() {
      Musician.Internal.Control_ResetValue(_ptr);
    }

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
