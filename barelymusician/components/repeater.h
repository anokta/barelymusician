#ifndef BARELYMUSICIAN_COMPONENTS_REPEATER_H_
#define BARELYMUSICIAN_COMPONENTS_REPEATER_H_

#include <optional>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/common/random.h"

namespace barely {

/// Repeater style.
enum class RepeaterStyle {
  kForward = 0,
  kBackward = 1,
  kRandom = 2,
};

/// Simple repeater that repeats notes in sequence.
class Repeater {
 public:
  /// Creates a new `Repeater`.
  ///
  /// @param musician Musician.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Repeater(Musician& musician, int process_order = 0) noexcept;

  /// Destroys `Repeater`.
  ~Repeater();

  /// Clears all pitches.
  void Clear() noexcept;

  /// Returns whether the repeater is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Pops the last pitch from the end.
  void Pop() noexcept;

  /// Pushes a new note to the end.
  ///
  /// @param pitch_or Note pitch or silence.
  /// @param length Note length.
  void Push(std::optional<double> pitch_or, int length = 1) noexcept;

  /// Sets the instrument.
  void SetInstrument(InstrumentRef instrument) noexcept;

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept;

  /// Sets the style.
  ///
  /// @param style Repeater style.
  void SetStyle(RepeaterStyle style) noexcept;

  /// Starts the repeater.
  ///
  /// @param pitch_shift Note pitch shift.
  void Start(double pitch_shift = 0.0) noexcept;

  /// Stop the repeater.
  void Stop() noexcept;

 private:
  // Updates the repeater.
  bool Update() noexcept;

  // Musician.
  Musician& musician_;

  // Performer.
  PerformerRef performer_;

  // Instrument.
  InstrumentRef instrument_;

  // List of pitches to play.
  std::vector<std::pair<std::optional<double>, int>> pitches_;

  // Style.
  RepeaterStyle style_ = RepeaterStyle::kForward;

  // Current index.
  int index_ = -1;

  // Pitch shift.
  double pitch_shift_ = 0.0;

  // Remaining length;
  int remaining_length_ = 0;

  // Random number generator.
  Random random_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_COMPONENTS_REPEATER_H_
