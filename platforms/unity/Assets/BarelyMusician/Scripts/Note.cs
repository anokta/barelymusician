using System;
using System.IO.Pipes;
using UnityEngine;

namespace Barely {
  public class Note {
    /// Constructs a new `Note`.
    ///
    /// @param ptr Pointer to control.
    public Note(IntPtr ptr) {
      _ptr = ptr;
    }

    ~Note() {
      Destroy();
    }

    /// Destroys the note.
    public void Destroy() {
      Musician.Internal.Note_Destroy(ref _ptr);
    }

    /// Returns a control.
    ///
    /// @param controlId Control identifier.
    /// @return Control.
    public Control GetControl(int controlId) {
      return Musician.Internal.Note_GetControl(_ptr, controlId);
    }

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
