#ifndef BARELYMUSICIAN_COMPONENTS_REPEATER_H_
#define BARELYMUSICIAN_COMPONENTS_REPEATER_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Repeater handle.
typedef struct BarelyRepeater* BarelyRepeaterHandle;

/// Repeater style enum alias.
typedef int32_t BarelyRepeaterStyle;

/// Clears all pitches.
///
/// @param repeater Repeater handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Clear(BarelyRepeaterHandle repeater);

/// Creates a new repeater.
///
/// @param musician Musician handle.
/// @param process_order Repeater process order.
/// @param out_repeater Output repeater handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Create(BarelyMusicianHandle musician, int32_t process_order,
                                         BarelyRepeaterHandle* out_repeater);

/// Destroys an repeater.
///
/// @param repeater Repeater handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Destroy(BarelyRepeaterHandle repeater);

/// Gets whether an repeater is playing or not.
///
/// @param repeater Repeater handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_IsPlaying(BarelyRepeaterHandle repeater, bool* out_is_playing);

/// Pops the last note from the end.
///
/// @param repeater Repeater handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Pop(BarelyRepeaterHandle repeater);

/// Pushes a new note to the end.
///
/// @param repeater Repeater handle.
/// @param pitch Note pitch.
/// @param length Note length.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Push(BarelyRepeaterHandle repeater, BarelyRational pitch,
                                       int32_t length);

/// Pushes silence to the end.
///
/// @param repeater Repeater handle.
/// @param length Note length.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_PushSilence(BarelyRepeaterHandle repeater, int32_t length);

/// Sets the instrument of an repeater.
///
/// @param repeater Repeater handle.
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_SetInstrument(BarelyRepeaterHandle repeater,
                                                BarelyInstrumentHandle instrument);

/// Sets the rate of an repeater.
///
/// @param repeater Repeater handle.
/// @param rate Rate in notes per beat.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_SetRate(BarelyRepeaterHandle repeater, BarelyRational rate);

/// Sets the style of an repeater.
///
/// @param repeater Repeater handle.
/// @param style Repeater style.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_SetStyle(BarelyRepeaterHandle repeater,
                                           BarelyRepeaterStyle style);

/// Starts the repeater.
///
/// @param repeater Repeater handle.
/// @param pitch_shift Note pitch shift.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Start(BarelyRepeaterHandle repeater, BarelyRational pitch_shift);

/// Stops the repeater.
///
/// @param repeater Repeater handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Stop(BarelyRepeaterHandle repeater);

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

  /// Non-copyable.
  Repeater(const Repeater& other) noexcept = delete;
  Repeater& operator=(const Repeater& other) noexcept = delete;

  /// Movable.
  Repeater(Repeater&& other) noexcept = default;
  Repeater& operator=(Repeater&& other) noexcept = default;

  /// Clears all pitches.
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
  void Push(std::optional<Rational> pitch_or, int length = 1) noexcept;

  /// Sets the instrument.
  void SetInstrument(Instrument* instrument) noexcept;

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(Rational rate) noexcept;

  /// Sets the style.
  ///
  /// @param style Repeater style.
  void SetStyle(RepeaterStyle style) noexcept;

  /// Starts the repeater.
  ///
  /// @param pitch_shift Note pitch shift.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Start(Rational pitch_shift = 0) noexcept;

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
  std::vector<std::pair<std::optional<Rational>, int>> pitches_;

  // Style.
  RepeaterStyle style_ = RepeaterStyle::kForward;

  // Current index.
  int index_ = -1;

  // Pitch shift.
  Rational pitch_shift_ = 0;

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
