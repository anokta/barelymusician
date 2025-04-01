#ifndef BARELYMUSICIAN_PRIVATE_ARPEGGIATOR_H_
#define BARELYMUSICIAN_PRIVATE_ARPEGGIATOR_H_

#include <barelymusician.h>

#include <vector>

#include "private/engine.h"
#include "private/instrument.h"
#include "private/performer.h"

/// Implementation of an arpeggiator.
struct BarelyArpeggiator {
 public:
  // Constructs a new `BarelyArpeggiator`.
  ///
  /// @param engine Engine.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit BarelyArpeggiator(BarelyEngine& engine) noexcept;

  /// Default destructor.
  ~BarelyArpeggiator() noexcept = default;

  /// Non-copyable.
  BarelyArpeggiator(const BarelyArpeggiator& other) noexcept = delete;
  BarelyArpeggiator& operator=(const BarelyArpeggiator& other) noexcept = delete;

  /// Movable.
  BarelyArpeggiator(BarelyArpeggiator&& other) noexcept = delete;
  BarelyArpeggiator& operator=(BarelyArpeggiator&& other) noexcept = delete;

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
  void SetInstrument(BarelyInstrument* instrument) noexcept;

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
  void SetStyle(BarelyArpeggiatorStyle style) noexcept;

 private:
  // Helper function to set the next note off.
  void SetNextNoteOff() noexcept;

  // Helper function to set the next note on.
  void SetNextNoteOn() noexcept;

  // Stop the arpeggiator.
  void Stop() noexcept;

  // Updates the arpeggiator.
  void Update() noexcept;

  // Pointer to engine.
  BarelyEngine* engine_ = nullptr;

  // Performer.
  BarelyPerformer performer_;

  // Task.
  BarelyTask task_;

  // Pointer to instrument.
  BarelyInstrument* instrument_ = nullptr;

  // Array of pitches to play.
  std::vector<float> pitches_;

  // Gate ratio.
  float gate_ratio_ = 1.0f;

  // Style.
  BarelyArpeggiatorStyle style_ = BarelyArpeggiatorStyle_kUp;

  // Current index.
  int index_ = -1;

  // Current pitch.
  float pitch_ = 0.0f;
};

#endif  // BARELYMUSICIAN_PRIVATE_ARPEGGIATOR_H_
