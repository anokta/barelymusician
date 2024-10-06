#ifndef BARELYMUSICIAN_COMPONENTS_REPEATER_H_
#define BARELYMUSICIAN_COMPONENTS_REPEATER_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Repeater alias.
typedef struct BarelyRepeater BarelyRepeater;

/// Repeater style enum alias.
typedef int32_t BarelyRepeaterStyle;

/// Clears all notes.
///
/// @param repeater Pointer to repeater.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Clear(BarelyRepeater* repeater);

/// Creates a new repeater.
///
/// @param musician Pointer to musician.
/// @param process_order Repeater process order.
/// @param out_repeater Output pointer to repeater.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Create(BarelyMusician* musician, int32_t process_order,
                                         BarelyRepeater** out_repeater);

/// Destroys an repeater.
///
/// @param repeater Pointer to repeater.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Destroy(BarelyRepeater* repeater);

/// Gets whether an repeater is playing or not.
///
/// @param repeater Pointer to repeater.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_IsPlaying(const BarelyRepeater* repeater, bool* out_is_playing);

/// Pops the last note from the end.
///
/// @param repeater Pointer to repeater.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Pop(BarelyRepeater* repeater);

/// Pushes a new note to the end.
///
/// @param repeater Pointer to repeater.
/// @param pitch Note pitch.
/// @param length Note length.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Push(BarelyRepeater* repeater, double pitch, int32_t length);

/// Pushes silence to the end.
///
/// @param repeater Pointer to repeater.
/// @param length Note length.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_PushSilence(BarelyRepeater* repeater, int32_t length);

/// Sets the instrument of an repeater.
///
/// @param repeater Pointer to repeater.
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_SetInstrument(BarelyRepeater* repeater,
                                                BarelyInstrument* instrument);

/// Sets the rate of an repeater.
///
/// @param repeater Pointer to repeater.
/// @param rate Rate in notes per beat.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_SetRate(BarelyRepeater* repeater, double rate);

/// Sets the style of an repeater.
///
/// @param repeater Pointer to repeater.
/// @param style Repeater style.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_SetStyle(BarelyRepeater* repeater, BarelyRepeaterStyle style);

/// Starts the repeater.
///
/// @param repeater Pointer to repeater.
/// @param pitch_offset Pitch offset.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Start(BarelyRepeater* repeater, double pitch_offset);

/// Stops the repeater.
///
/// @param repeater Pointer to repeater.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Stop(BarelyRepeater* repeater);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
#include <optional>
#include <utility>
#include <vector>

#include "barelymusician/common/random.h"

namespace barely {

/// Repeater style.
enum class RepeaterStyle : BarelyRepeaterStyle {
  kForward = 0,
  kBackward = 1,
  kRandom = 2,
};

/// Simple repeater that repeats notes in sequence.
class Repeater {
 public:
  // Constructs a new `Repeater`.
  ///
  /// @param musician Musician pointer.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Repeater(MusicianPtr musician, int process_order = 0) noexcept;

  /// Destroys `Repeater`.
  ~Repeater() noexcept;

  /// Non-copyable.
  Repeater(const Repeater& other) noexcept = delete;
  Repeater& operator=(const Repeater& other) noexcept = delete;

  /// Movable.
  Repeater(Repeater&& other) noexcept = default;
  Repeater& operator=(Repeater&& other) noexcept = default;

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
  /// @param instrument Optional instrument.
  void SetInstrument(std::optional<InstrumentPtr> instrument) noexcept;

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

  // Performer.
  Performer performer_;

  // Instrument.
  std::optional<InstrumentPtr> instrument_ = std::nullopt;

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

  // Task.
  Task task_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPONENTS_REPEATER_H_
