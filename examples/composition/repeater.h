#ifndef BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_
#define BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_

#include <barelymusician.h>

#include <optional>
#include <utility>
#include <vector>

namespace barely::examples {

/// Repeater modes.
enum class RepeaterMode {
  /// Forward.
  kForward = 0,
  /// Backward.
  kBackward,
  /// Random.
  kRandom,
};

/// Class that plays a repeating sequence of instrument notes.
struct Repeater {
 public:
  // Constructs a new `Repeater`.
  ///
  /// @param engine Engine.
  /// @param instrument Instrument.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Repeater(Engine& engine, Instrument instrument) noexcept;

  ~Repeater() noexcept { engine_.DestroyPerformer(performer_); }

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

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept;

  /// Sets the style.
  ///
  /// @param style Repeater style.
  void SetStyle(RepeaterMode style) noexcept;

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
  Engine& engine_;

  // Instrument.
  Instrument instrument_;

  // Performer.
  Performer performer_;

  // Task.
  Task task_;

  // Array of pitches to play.
  std::vector<std::pair<std::optional<float>, int>> pitches_;

  // Mode.
  RepeaterMode mode_ = RepeaterMode::kForward;

  // Current index.
  int index_ = -1;

  // Pitch offset.
  float pitch_offset_ = 0.0;

  // Remaining length;
  int remaining_length_ = 0;
};

}  // namespace barely::examples

#endif  // BARELYMUSICIAN_EXAMPLES_COMPOSITION_REPEATER_H_
