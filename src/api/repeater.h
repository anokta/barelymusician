#ifndef BARELYMUSICIAN_API_REPEATER_H_
#define BARELYMUSICIAN_API_REPEATER_H_

#include <barelymusician.h>

#include <optional>
#include <utility>
#include <vector>

#include "api/engine.h"
#include "api/instrument.h"
#include "api/performer.h"

/// Implementation of a repeater.
struct BarelyRepeater {
 public:
  // Constructs a new `BarelyRepeater`.
  ///
  /// @param engine Engine.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit BarelyRepeater(BarelyEngine& engine) noexcept;

  /// Destroys `BarelyRepeater`.
  ~BarelyRepeater() noexcept;

  /// Non-copyable.
  BarelyRepeater(const BarelyRepeater& other) noexcept = delete;
  BarelyRepeater& operator=(const BarelyRepeater& other) noexcept = delete;

  /// Movable.
  BarelyRepeater(BarelyRepeater&& other) noexcept = delete;
  BarelyRepeater& operator=(BarelyRepeater&& other) noexcept = delete;

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
  void SetInstrument(BarelyInstrument* instrument) noexcept;

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept;

  /// Sets the style.
  ///
  /// @param style Repeater style.
  void SetStyle(BarelyRepeaterStyle style) noexcept;

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

  // Pointer to engine.
  BarelyEngine* engine_ = nullptr;

  // Performer.
  BarelyPerformer performer_;

  // Pointer to instrument.
  BarelyInstrument* instrument_ = nullptr;

  // Array of pitches to play.
  std::vector<std::pair<std::optional<float>, int>> pitches_;

  // Style.
  BarelyRepeaterStyle style_ = BarelyRepeaterStyle_kForward;

  // Current index.
  int index_ = -1;

  // Pitch offset.
  float pitch_offset_ = 0.0;

  // Remaining length;
  int remaining_length_ = 0;
};

#endif  // BARELYMUSICIAN_API_REPEATER_H_
