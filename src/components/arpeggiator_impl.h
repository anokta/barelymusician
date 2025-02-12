#ifndef BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_IMPL_H_
#define BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_IMPL_H_

#include <vector>

#include "barelycomposer.h"
#include "common/random_impl.h"
#include "internal/engine_impl.h"
#include "internal/instrument_impl.h"
#include "internal/performer_impl.h"

namespace barely::internal {

/// Class that implements an arpeggiator.
class ArpeggiatorImpl {
 public:
  // Constructs a new `ArpeggiatorImpl`.
  ///
  /// @param engine Engine.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit ArpeggiatorImpl(EngineImpl& engine) noexcept;

  /// Destroys `ArpeggiatorImpl`.
  ~ArpeggiatorImpl() noexcept;

  /// Non-copyable.
  ArpeggiatorImpl(const ArpeggiatorImpl& other) noexcept = delete;
  ArpeggiatorImpl& operator=(const ArpeggiatorImpl& other) noexcept = delete;

  /// Movable.
  ArpeggiatorImpl(ArpeggiatorImpl&& other) noexcept = delete;
  ArpeggiatorImpl& operator=(ArpeggiatorImpl&& other) noexcept = delete;

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
  void SetInstrument(InstrumentImpl* instrument) noexcept;

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
  /// @param style ArpeggiatorImpl style.
  void SetStyle(ArpeggiatorStyle style) noexcept;

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
  EngineImpl* engine_ = nullptr;

  // Performer.
  PerformerImpl* performer_ = nullptr;

  // Task.
  PerformerImpl::TaskImpl* task_ = nullptr;

  // Instrument.
  InstrumentImpl* instrument_ = nullptr;

  // Array of pitches to play.
  std::vector<float> pitches_;

  // Gate ratio.
  float gate_ratio_ = 1.0f;

  // Style.
  ArpeggiatorStyle style_ = ArpeggiatorStyle::kUp;

  // Current index.
  int index_ = -1;

  // Current pitch.
  float pitch_ = 0.0f;

  // Random number generator.
  RandomImpl random_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_IMPL_H_
