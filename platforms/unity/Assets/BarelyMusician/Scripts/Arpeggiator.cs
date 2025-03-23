using System;
using System.Runtime.InteropServices.ComTypes;
using System.Threading;
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

  /// A representation of a simple arpeggiator that can be attached to a musical instrument to play
  /// notes in sequence.
  [RequireComponent(typeof(Instrument))]
  public class Arpeggiator : MonoBehaviour {
    /// Gate ratio.
    [Range(0.0f, 1.0f)]
    public float GateRatio = 0.5f;

    /// Rate.
    [Range(0.0f, 8.0f)]
    public double Rate = 1.0;

    /// Syle.
    public ArpeggiatorStyle Style = ArpeggiatorStyle.UP;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Engine.Internal.Arpeggiator_IsPlaying(_handle); }
    }

    /// Returns whether a note is on or not.
    ///
    /// @param pitch Note pitch
    /// @return True if on, false otherwise.
    public bool IsNoteOn(float pitch) {
      return Engine.Internal.Arpeggiator_IsNoteOn(_handle, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Engine.Internal.Arpeggiator_SetAllNotesOff(_handle);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(float pitch) {
      Engine.Internal.Arpeggiator_SetNoteOff(_handle, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOn(float pitch) {
      Engine.Internal.Arpeggiator_SetNoteOn(_handle, pitch);
    }

    private void OnEnable() {
      Engine.Internal.Component_Create(this, ref _handle);
      _instrument = GetComponent<Instrument>();
      _instrument.OnInstrumentCreate += OnInstrumentCreate;
      _instrument.OnInstrumentDestroy += OnInstrumentDestroy;
      Engine.Internal.Arpeggiator_SetInstrument(_handle, _instrument);
      Update();
    }

    private void OnDisable() {
      Engine.Internal.Component_Destroy(this, ref _handle);
      _instrument.OnInstrumentCreate -= OnInstrumentCreate;
      _instrument.OnInstrumentDestroy -= OnInstrumentDestroy;
      _instrument = null;
    }

    private void Update() {
      Engine.Internal.Arpeggiator_SetGateRatio(_handle, GateRatio);
      Engine.Internal.Arpeggiator_SetRate(_handle, Rate);
      Engine.Internal.Arpeggiator_SetStyle(_handle, Style);
    }

    private void OnInstrumentCreate() {
      Engine.Internal.Arpeggiator_SetInstrument(_handle, _instrument);
    }

    private void OnInstrumentDestroy() {
      Engine.Internal.Arpeggiator_SetInstrument(_handle, null);
    }

    // Current instrument.
    private Instrument _instrument = null;

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
