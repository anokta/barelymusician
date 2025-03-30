#ifndef BARELYMUSICIAN_PRIVATE_REPEATER_H_
#define BARELYMUSICIAN_PRIVATE_REPEATER_H_

#include <optional>
#include <utility>
#include <vector>

#include "barelymusician.h"
#include "private/engine.h"
#include "private/instrument.h"
#include "private/performer.h"

namespace barely {

/// Class that implements a repeater.
class RepeaterImpl {
 public:
  // Constructs a new `RepeaterImpl`.
  ///
  /// @param engine Engine.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit RepeaterImpl(EngineImpl& engine) noexcept;

  /// Destroys `RepeaterImpl`.
  ~RepeaterImpl() noexcept;

  /// Non-copyable.
  RepeaterImpl(const RepeaterImpl& other) noexcept = delete;
  RepeaterImpl& operator=(const RepeaterImpl& other) noexcept = delete;

  /// Movable.
  RepeaterImpl(RepeaterImpl&& other) noexcept = delete;
  RepeaterImpl& operator=(RepeaterImpl&& other) noexcept = delete;

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
  void SetInstrument(InstrumentImpl* instrument) noexcept;

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept;

  /// Sets the style.
  ///
  /// @param style RepeaterImpl style.
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
  EngineImpl* engine_ = nullptr;

  // Performer.
  PerformerImpl* performer_ = nullptr;

  // Instrument.
  InstrumentImpl* instrument_ = nullptr;

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
};

}  // namespace barely

struct BarelyRepeater : public barely::RepeaterImpl {};
static_assert(sizeof(BarelyRepeater) == sizeof(barely::RepeaterImpl));

#endif  // BARELYMUSICIAN_PRIVATE_REPEATER_H_
