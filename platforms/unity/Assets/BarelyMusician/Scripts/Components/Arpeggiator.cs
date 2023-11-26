using System;
using UnityEngine;

namespace Barely {
  public enum ArpeggiatorStyle {
    [InspectorName("Up")] UP = 0,
    [InspectorName("Down")] DOWN = 1,
    // UpDown = 2,
    // DownUp = 3,
    // UpAndDown = 4,
    // DownAndUp = 5,
    // PinkyUp = 6,
    // ThumbUp = 7,
    [InspectorName("Random")] RANDOM = 8,
  }

  /// A represantion of a simple arpeggiator hat can be attached to a musical instrument to play
  /// notes in sequence.
  [RequireComponent(typeof(Instrument))]
  public class Arpeggiator : MonoBehaviour {
    public int ProcessOrder = 0;

    /// Gate ratio.
    [Range(0.0f, 1.0f)]
    public double GateRatio = 0.5;

    /// Rate.
    [Range(0.0f, 8.0f)]
    public double Rate = 1.0;

    /// Syle.
    public ArpeggiatorStyle Style = ArpeggiatorStyle.UP;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Arpeggiator_IsPlaying(_handle); }
    }

    /// Returns whether a note is on or not.
    ///
    /// @param Note pitch.
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Arpeggiator_IsNoteOn(_handle, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Arpeggiator_SetAllNotesOff(_handle);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Internal.Arpeggiator_SetNoteOff(_handle, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch) {
      Musician.Internal.Arpeggiator_SetNoteOn(_handle, pitch);
    }

    protected virtual void Update() {
      if (_handle == IntPtr.Zero && GetComponent<Instrument>().enabled) {
        Musician.Internal.Component_Create(
            this, Instrument.Internal.GetInstrumentHandle(GetComponent<Instrument>()), ref _handle);
      }
      Musician.Internal.Arpeggiator_SetGateRatio(_handle, GateRatio);
      Musician.Internal.Arpeggiator_SetRate(_handle, Rate);
      Musician.Internal.Arpeggiator_SetStyle(_handle, Style);
    }

    protected virtual void OnDisable() {
      Musician.Internal.Component_Destroy(this, ref _handle);
    }

    // Handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
