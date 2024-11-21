using System;
using System.Runtime.InteropServices.ComTypes;
using System.Threading;
using UnityEngine;

namespace Barely {
  public enum RepeaterStyle {
    [InspectorName("Forward")] FORWARD = 0,
    [InspectorName("Backward")] BACKWARD = 1,
    [InspectorName("Random")] RANDOM = 8,
  }

  /// A representation of a simple repeater that can be attached to a musical instrument to play a
  /// repeating sequence of notes.
  [RequireComponent(typeof(Instrument))]
  public class Repeater : MonoBehaviour {
    public int ProcessOrder = 0;

    /// Rate.
    [Range(0.0f, 8.0f)]
    public double Rate = 1.0;

    /// Syle.
    public RepeaterStyle Style = RepeaterStyle.FORWARD;

    /// True if playing, false otherwise.
    public bool IsPlaying {
      get { return Musician.Internal.Repeater_IsPlaying(_handle); }
    }

    /// Pops the last note from the end.
    public void Pop() {
      Musician.Internal.Repeater_Pop(_handle);
    }

    /// Pushes a new note to the end.
    ///
    /// @param pitchOr Note pitch or silence.
    /// @param length Note length in beats.
    public void Push(double? pitchOr, int length = 1) {
      Musician.Internal.Repeater_Push(_handle, pitchOr, length);
    }

    /// Starts the repeater.
    ///
    /// @param pitchOffset Pitch offset.
    public void Play(double pitchOffset = 0.0) {
      Musician.Internal.Repeater_Start(_handle, pitchOffset);
    }

    /// Stops the repeater.
    public void Stop() {
      Musician.Internal.Repeater_Stop(_handle);
    }

    private void OnEnable() {
      Musician.Internal.Component_Create(this, ref _handle);
      _instrument = GetComponent<Instrument>();
      _instrument.OnInstrumentCreate += OnInstrumentCreate;
      _instrument.OnInstrumentDestroy += OnInstrumentDestroy;
      Musician.Internal.Repeater_SetInstrument(_handle, _instrument);
      Update();
    }

    private void OnDisable() {
      Musician.Internal.Component_Destroy(this, ref _handle);
      _instrument.OnInstrumentCreate -= OnInstrumentCreate;
      _instrument.OnInstrumentDestroy -= OnInstrumentDestroy;
      _instrument = null;
    }

    private void Update() {
      Musician.Internal.Repeater_SetRate(_handle, Rate);
      Musician.Internal.Repeater_SetStyle(_handle, Style);
    }

    private void OnInstrumentCreate() {
      Musician.Internal.Repeater_SetInstrument(_handle, _instrument);
    }

    private void OnInstrumentDestroy() {
      Musician.Internal.Repeater_SetInstrument(_handle, null);
    }

    // Current instrument.
    private Instrument _instrument = null;

    // Raw handle.
    private IntPtr _handle = IntPtr.Zero;
  }
}  // namespace Barely
