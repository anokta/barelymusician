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
      get { return Musician.Internal.Arpeggiator_IsPlaying(_ptr); }
    }

    /// Returns whether a note is on or not.
    ///
    /// @param Note pitch.
    /// @return True if on, false otherwise.
    public bool IsNoteOn(double pitch) {
      return Musician.Internal.Arpeggiator_IsNoteOn(_ptr, pitch);
    }

    /// Sets all notes off.
    public void SetAllNotesOff() {
      Musician.Internal.Arpeggiator_SetAllNotesOff(_ptr);
    }

    /// Sets a note off.
    ///
    /// @param pitch Note pitch.
    public void SetNoteOff(double pitch) {
      Musician.Internal.Arpeggiator_SetNoteOff(_ptr, pitch);
    }

    /// Sets a note on.
    ///
    /// @param pitch Note pitch.
    /// @param intensity Note intensity.
    public void SetNoteOn(double pitch) {
      Musician.Internal.Arpeggiator_SetNoteOn(_ptr, pitch);
    }

    private void OnEnable() {
      Musician.Internal.Component_Create(this, ref _ptr);
      _instrument = GetComponent<Instrument>();
      _instrument.OnInstrumentCreate += OnInstrumentCreate;
      _instrument.OnInstrumentDestroy += OnInstrumentDestroy;
      Musician.Internal.Arpeggiator_SetInstrument(_ptr, _instrument);
    }

    private void OnDisable() {
      Musician.Internal.Component_Destroy(this, ref _ptr);
      _instrument.OnInstrumentCreate -= OnInstrumentCreate;
      _instrument.OnInstrumentDestroy -= OnInstrumentDestroy;
      _instrument = null;
    }

    private void Update() {
      Musician.Internal.Arpeggiator_SetGateRatio(_ptr, GateRatio);
      Musician.Internal.Arpeggiator_SetRate(_ptr, Rate);
      Musician.Internal.Arpeggiator_SetStyle(_ptr, Style);
    }

    private void OnInstrumentCreate() {
      Musician.Internal.Arpeggiator_SetInstrument(_ptr, _instrument);
    }

    private void OnInstrumentDestroy() {
      Musician.Internal.Arpeggiator_SetInstrument(_ptr, null);
    }

    // Current instrument.
    private Instrument _instrument = null;

    // Raw pointer.
    private IntPtr _ptr = IntPtr.Zero;
  }
}  // namespace Barely
