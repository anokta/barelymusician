#ifndef BARELYMUSICIAN_API_ARPEGGIATOR_H_
#define BARELYMUSICIAN_API_ARPEGGIATOR_H_

#include <barelymusician.h>

#include <vector>

#include "api/performer.h"
#include "api/task.h"

namespace barely {

/// Implementation of an arpeggiator.
struct Arpeggiator {
 public:
  // Constructs a new `Arpeggiator`.
  ///
  /// @param engine Engine.
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Arpeggiator(BarelyEngine& engine, BarelyInstrument& instrumnet) noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  bool IsNoteOn(float pitch) const noexcept;

  /// Returns whether the arpeggiator is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Sets all notes off.
  void SetAllNotesOff() noexcept;

  /// Sets the gate ratio.
  ///
  /// @param gate_ratio Gate ratio.
  void SetGateRatio(float gate_ratio) noexcept;

  /// Sets the mode.
  ///
  /// @param mode Arpeggiator mode.
  void SetMode(BarelyArpMode mode) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOff(float pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(float pitch) noexcept;

  /// Sets the rate.
  ///
  /// @param gate_ratio Gate ratio.
  /// @param rate Rate in notes per beat.
  void SetRate(float gate_ratio, float rate) noexcept;

 private:
  // Helper function to set the next note off.
  void SetNextNoteOff() noexcept;

  // Helper function to set the next note on.
  void SetNextNoteOn() noexcept;

  // Stop the arpeggiator.
  void Stop() noexcept;

  // Updates the arpeggiator.
  void Update() noexcept;

  // Engine.
  BarelyEngine& engine_;

  // Instrument.
  BarelyInstrument& instrument_;

  // Performer.
  BarelyPerformer performer_;

  // Task.
  BarelyTask task_;

  // Array of pitches to play.
  std::vector<float> pitches_;

  // Mode.
  BarelyArpMode mode_ = BarelyArpMode_kNone;

  // Current index.
  int index_ = -1;

  // Current pitch.
  float pitch_ = 0.0f;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_API_ARPEGGIATOR_H_
