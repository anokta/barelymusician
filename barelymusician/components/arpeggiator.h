#ifndef BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
#define BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_

#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"

namespace barely {

/// Arpeggiator style.
enum class ArpeggiatorStyle {
  kUp = 0,
  kDown = 1,
  // kUpDown = 2,
  // kDownUp = 3,
  // kUpAndDown = 4,
  // kDownAndUp = 5,
  // kPinkyUp = 6,
  // kThumbUp = 7,
  kRandom = 8,
};

/// Simple arpeggiator that plays notes in sequence.
class Arpeggiator {
 public:
  /// Creates a new `Arpeggiator`.
  ///
  /// @param musician Musician.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Arpeggiator(Musician& musician, int process_order = 0) noexcept;

  /// Destroys `Arpeggiator`.
  ~Arpeggiator();

  /// Returns whether a note is on or not.
  ///
  /// @return True if on, false otherwise.
  bool IsNoteOn(double pitch) const noexcept;

  /// Returns whether the arpeggiator is playing or not.
  ///
  /// @return True if playing, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool IsPlaying() const noexcept;

  /// Sets all notes off.
  void SetAllNotesOff() noexcept;

  /// Sets the gate ratio.
  ///
  /// @param gate Gate ratio.
  void SetGateRatio(double gate_ratio) noexcept;

  /// Sets the instrument.
  void SetInstrument(InstrumentHandle instrument) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOff(double pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch) noexcept;

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

  // Performer.
  Performer performer_;

  // Instrument.
  InstrumentHandle instrument_;

  // List of pitches to play.
  std::vector<double> pitches_;

  // Gate ratio.
  double gate_ratio_ = 1.0;

  // Style.
  ArpeggiatorStyle style_ = ArpeggiatorStyle::kUp;

  // Current index.
  int index_ = -1;

  // Random number generator.
  Random random_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
