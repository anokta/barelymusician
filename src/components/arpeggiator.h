#ifndef BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
#define BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_

#include <vector>

#include "barelycomposer.h"
#include "common/random.h"
#include "engine/instrument.h"
#include "engine/musician.h"
#include "engine/performer.h"

namespace barely::internal {

/// Class that wraps an arpeggiator.
class Arpeggiator {
 public:
  // Constructs a new `Arpeggiator`.
  ///
  /// @param musician Musician.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Arpeggiator(Musician& musician) noexcept;

  /// Destroys `Arpeggiator`.
  ~Arpeggiator() noexcept;

  /// Non-copyable.
  Arpeggiator(const Arpeggiator& other) noexcept = delete;
  Arpeggiator& operator=(const Arpeggiator& other) noexcept = delete;

  /// Movable.
  Arpeggiator(Arpeggiator&& other) noexcept = delete;
  Arpeggiator& operator=(Arpeggiator&& other) noexcept = delete;

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
  /// @param gate Gate ratio.
  void SetGateRatio(float gate_ratio) noexcept;

  /// Sets the instrument.
  ///
  /// @param instrument Pointer to instrument.
  void SetInstrument(Instrument* instrument) noexcept;

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
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept;

  /// Sets the style.
  ///
  /// @param style Arpeggiator style.
  void SetStyle(ArpeggiatorStyle style) noexcept;

 private:
  // Stop the arpeggiator.
  void Stop() noexcept;

  // Updates the arpeggiator.
  void Update() noexcept;

  // Musician.
  Musician* musician_ = nullptr;

  // Performer.
  Performer* performer_ = nullptr;

  // Instrument.
  Instrument* instrument_ = nullptr;

  // Array of pitches to play.
  std::vector<float> pitches_;

  // Gate ratio.
  float gate_ratio_ = 1.0f;

  // Style.
  ArpeggiatorStyle style_ = ArpeggiatorStyle::kUp;

  // Current index.
  int index_ = -1;

  // Note off task.
  Performer::Task* note_off_task_;

  // Random number generator.
  Random random_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
