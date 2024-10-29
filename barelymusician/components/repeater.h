#ifndef BARELYMUSICIAN_COMPONENTS_REPEATER_H_
#define BARELYMUSICIAN_COMPONENTS_REPEATER_H_

#include <optional>
#include <utility>
#include <vector>

#include "barelycomposer.h"
#include "barelymusician/common/random.h"
#include "barelymusician/internal/instrument_controller.h"
#include "barelymusician/internal/musician.h"
#include "barelymusician/internal/performer.h"

namespace barely::internal {

/// Class that wraps a repeater.
class Repeater {
 public:
  // Constructs a new `Repeater`.
  ///
  /// @param musician Musician.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Repeater(Musician& musician, int process_order = 0) noexcept;

  /// Destroys `Repeater`.
  ~Repeater() noexcept;

  /// Non-copyable.
  Repeater(const Repeater& other) noexcept = delete;
  Repeater& operator=(const Repeater& other) noexcept = delete;

  /// Movable.
  Repeater(Repeater&& other) noexcept = delete;
  Repeater& operator=(Repeater&& other) noexcept = delete;

  /// Clears all notes.
  void Clear() noexcept;

  /// Returns whether the repeater is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Pops the last note from the end.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Pop() noexcept;

  /// Pushes a new note to the end.
  ///
  /// @param pitch_or Note pitch or silence.
  /// @param length Note length.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Push(std::optional<double> pitch_or, int length = 1) noexcept;

  /// Sets the instrument.
  ///
  /// @param instrument Pointer to instrument.
  void SetInstrument(InstrumentController* instrument) noexcept;

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
  /// @param pitch_offset Pitch offset.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Start(double pitch_offset = 0.0) noexcept;

  /// Stop the repeater.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Stop() noexcept;

 private:
  // Updates the repeater.
  bool Update() noexcept;

  // Musician.
  Musician& musician_;

  // Performer.
  Performer* performer_ = nullptr;

  // Instrument.
  InstrumentController* instrument_ = nullptr;

  // Array of pitches to play.
  std::vector<std::pair<std::optional<double>, int>> pitches_;

  // Style.
  RepeaterStyle style_ = RepeaterStyle::kForward;

  // Current index.
  int index_ = -1;

  // Pitch offset.
  double pitch_offset_ = 0.0;

  // Remaining length;
  int remaining_length_ = 0;

  // Random number generator.
  Random random_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_COMPONENTS_REPEATER_H_
