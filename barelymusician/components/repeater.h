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
  /// Destroys `Repeater`.
  ~Repeater() noexcept;

  /// Clears all pitches.
  void Clear() noexcept;

  /// Returns whether the repeater is playing or not.
  ///
  /// @return True if playing, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool IsPlaying() const noexcept;

  /// Pops the last pitch from the end.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Pop() noexcept;

  /// Pushes a new note to the end.
  ///
  /// @param pitch_or Note pitch or silence.
  /// @param length Note length.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Push(std::optional<double> pitch_or, int length = 1) noexcept;

  /// Sets the instrument.
  void SetInstrument(Instrument* instrument) noexcept;

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
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Start(double pitch_shift = 0.0) noexcept;

  /// Stop the repeater.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Stop() noexcept;

 private:
  // Ensures that the component can only be created by `Musician`.
  friend class Musician;

  // Creates a new `Repeater` with a given `musician` and `process_order`.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Repeater(Musician& musician, int process_order = 0) noexcept;

  // Updates the repeater.
  bool Update() noexcept;

  // Performer.
  Performer performer_;

  // Instrument.
  Instrument* instrument_ = nullptr;

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
