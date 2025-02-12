#ifndef BARELYMUSICIAN_COMPONENTS_REPEATER_H_
#define BARELYMUSICIAN_COMPONENTS_REPEATER_H_

#include <optional>
#include <utility>
#include <vector>

#include "barelycomposer.h"
#include "common/random.h"
#include "engine/engine.h"
#include "engine/instrument.h"
#include "engine/performer.h"

namespace barely::internal {

/// Class that wraps a repeater.
class Repeater {
 public:
  // Constructs a new `Repeater`.
  ///
  /// @param engine Engine.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Repeater(Engine& engine) noexcept;

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
  void Push(std::optional<float> pitch_or, int length = 1) noexcept;

  /// Sets the instrument.
  ///
  /// @param instrument Pointer to instrument.
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
  /// @param pitch_offset Pitch offset.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Start(float pitch_offset = 0.0) noexcept;

  /// Stop the repeater.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Stop() noexcept;

 private:
  void OnBeat() noexcept;

  // Updates the repeater.
  bool Update() noexcept;

  // Engine.
  Engine* engine_ = nullptr;

  // Performer.
  Performer* performer_ = nullptr;

  // Instrument.
  Instrument* instrument_ = nullptr;

  // Array of pitches to play.
  std::vector<std::pair<std::optional<float>, int>> pitches_;

  // Style.
  RepeaterStyle style_ = RepeaterStyle::kForward;

  // Current index.
  int index_ = -1;

  // Pitch offset.
  float pitch_offset_ = 0.0;

  // Remaining length;
  int remaining_length_ = 0;

  // Random number generator.
  Random random_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_COMPONENTS_REPEATER_H_
