#ifndef BARELYMUSICIAN_BARELYCOMPOSER_H_
#define BARELYMUSICIAN_BARELYCOMPOSER_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Arpeggiator style enum alias.
typedef int32_t BarelyArpeggiatorStyle;

/// Arpeggiator style enum values.
// TODO(#142): Add more arpeggiator styles.
enum BarelyArpeggiatorStyle_Values {
  /// Up.
  BarelyArpeggiatorStyle_kUp = 0,
  /// Down.
  BarelyArpeggiatorStyle_kDown,
  /// Random.
  BarelyArpeggiatorStyle_kRandom,
  /// Number of arpeggiator styles.
  BarelyArpeggiatorStyle_kCount,
};

/// Repeater style enum alias.
typedef int32_t BarelyRepeaterStyle;

/// Repeater style enum values.
enum BarelyRepeaterStyle_Values {
  /// Forward.
  BarelyRepeaterStyle_kForward = 0,
  /// Backward.
  BarelyRepeaterStyle_kBackward,
  /// Random.
  BarelyRepeaterStyle_kRandom,
  /// Number of repeater styles.
  BarelyRepeaterStyle_kCount,
};

/// A musical scale.
typedef struct BarelyScale {
  /// Array of note pitches relative to the root note pitch.
  const double* pitches;

  /// Number of note pitches.
  int32_t pitch_count;

  /// Root note pitch of the scale.
  double root_pitch;

  /// Mode index.
  int32_t mode;
} BarelyScale;

/// Arpeggiator handle alias.
typedef struct BarelyArpeggiator* BarelyArpeggiatorHandle;

/// Repeater handle alias.
typedef struct BarelyRepeater* BarelyRepeaterHandle;

/// Creates a new arpeggiator.
///
/// @param musician Musician handle.
/// @param process_order Arpeggiator process order.
/// @param out_arpeggiator Output arpeggiator handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_Create(BarelyMusician* musician, int32_t process_order,
                                            BarelyArpeggiatorHandle* out_arpeggiator);

/// Destroys an arpeggiator.
///
/// @param arpeggiator Arpeggiator handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_Destroy(BarelyArpeggiatorHandle arpeggiator);

/// Gets whether an arpeggiator note is on or not.
///
/// @param arpeggiator Arpeggiator handle.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_IsNoteOn(BarelyArpeggiatorHandle arpeggiator, double pitch,
                                              bool* out_is_note_on);

/// Gets whether an arpeggiator is playing or not.
///
/// @param arpeggiator Arpeggiator handle.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_IsPlaying(BarelyArpeggiatorHandle arpeggiator,
                                               bool* out_is_playing);

/// Sets all arpeggiator notes off.
///
/// @param arpeggiator Arpeggiator handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetAllNotesOff(BarelyArpeggiatorHandle arpeggiator);

/// Sets the gate ratio of an arpeggiator.
///
/// @param arpeggiator Arpeggiator handle.
/// @param gate_ratio Gate ratio.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetGateRatio(BarelyArpeggiatorHandle arpeggiator,
                                                  double gate_ratio);

/// Sets the instrument of an arpeggiator.
///
/// @param arpeggiator Arpeggiator handle.
/// @param instrument Instrument handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetInstrument(BarelyArpeggiatorHandle arpeggiator,
                                                   BarelyInstrumentHandle instrument);

/// Sets an arpeggiator note off.
///
/// @param arpeggiator Arpeggiator handle.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetNoteOff(BarelyArpeggiatorHandle arpeggiator, double pitch);

/// Sets an arpeggiator note on.
///
/// @param arpeggiator Arpeggiator handle.
/// @param pitch Note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetNoteOn(BarelyArpeggiatorHandle arpeggiator, double pitch);

/// Sets the rate of an arpeggiator.
///
/// @param arpeggiator Arpeggiator handle.
/// @param rate Rate in notes per beat.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetRate(BarelyArpeggiatorHandle arpeggiator, double rate);

/// Sets the style of an arpeggiator.
///
/// @param arpeggiator Arpeggiator handle.
/// @param style Arpeggiator style.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetStyle(BarelyArpeggiatorHandle arpeggiator,
                                              BarelyArpeggiatorStyle style);

/// Clears all notes.
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
BARELY_EXPORT bool BarelyRepeater_Create(BarelyMusician* musician, int32_t process_order,
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
BARELY_EXPORT bool BarelyRepeater_Push(BarelyRepeaterHandle repeater, double pitch, int32_t length);

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
BARELY_EXPORT bool BarelyRepeater_SetRate(BarelyRepeaterHandle repeater, double rate);

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
/// @param pitch_offset Pitch offset.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Start(BarelyRepeaterHandle repeater, double pitch_offset);

/// Stops the repeater.
///
/// @param repeater Repeater handle.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyRepeater_Stop(BarelyRepeaterHandle repeater);

/// Gets a scale note pitch for a given degree.
///
/// @param scale Pointer to scale.
/// @param degree Scale degree.
/// @param out_pitch Output note pitch.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyScale_GetPitch(const BarelyScale* scale, int32_t degree,
                                        double* out_pitch);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
#include <cassert>
#include <cstdint>
#include <optional>
#include <span>

namespace barely {

/// Arpeggiator style enum.
enum class ArpeggiatorStyle : BarelyArpeggiatorStyle {
  /// Up.
  kUp = BarelyArpeggiatorStyle_kUp,
  /// Down.
  kDown = BarelyArpeggiatorStyle_kDown,
  /// Random.
  kRandom = BarelyArpeggiatorStyle_kRandom,
  /// Number of arpeggiator styles.
  kCount = BarelyArpeggiatorStyle_kCount,
};

/// Repeater style enum.
enum class RepeaterStyle : BarelyRepeaterStyle {
  /// Forward.
  kForward = BarelyRepeaterStyle_kForward,
  /// Backward.
  kBackward = BarelyRepeaterStyle_kBackward,
  /// Random.
  kRandom = BarelyRepeaterStyle_kRandom,
  /// Number of repeater styles.
  kCount = BarelyRepeaterStyle_kCount,
};

/// Arpeggiator handle.
class ArpeggiatorHandle : public HandleWrapper<BarelyArpeggiatorHandle> {
 public:
  /// Creates a new `ArpeggiatorHandle`.
  ///
  /// @param musician Musician handle.
  /// @param process_order Process order.
  /// @return Arpeggiator handle.
  [[nodiscard]] static ArpeggiatorHandle Create(MusicianHandle musician,
                                                int process_order = 0) noexcept {
    BarelyArpeggiatorHandle arpeggiator = nullptr;
    [[maybe_unused]] const bool success =
        BarelyArpeggiator_Create(musician, static_cast<int32_t>(process_order), &arpeggiator);
    assert(success);
    return ArpeggiatorHandle(arpeggiator);
  }

  /// Destroys a `ArpeggiatorHandle`.
  ///
  /// @param arpeggiator Arpeggiator handle.
  static void Destroy(ArpeggiatorHandle arpeggiator) noexcept {
    BarelyArpeggiator_Destroy(arpeggiator);
  }

  /// Default constructor.
  constexpr ArpeggiatorHandle() noexcept = default;

  /// Creates a new `ArpeggiatorHandle` from a raw handle.
  ///
  /// @param arpeggiator Raw handle to arpeggiator.
  explicit constexpr ArpeggiatorHandle(BarelyArpeggiatorHandle arpeggiator) noexcept
      : HandleWrapper(arpeggiator) {}

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  bool IsNoteOn(double pitch) const noexcept {
    bool is_note_on = false;
    [[maybe_unused]] const bool success = BarelyArpeggiator_IsNoteOn(*this, pitch, &is_note_on);
    assert(success);
    return is_note_on;
  }

  /// Returns whether the arpeggiator is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const bool success = BarelyArpeggiator_IsPlaying(*this, &is_playing);
    assert(success);
    return is_playing;
  }

  /// Sets all notes off.
  void SetAllNotesOff() noexcept {
    [[maybe_unused]] const bool success = BarelyArpeggiator_SetAllNotesOff(*this);
    assert(success);
  }

  /// Sets the gate ratio.
  ///
  /// @param gate Gate ratio.
  void SetGateRatio(double gate_ratio) noexcept {
    [[maybe_unused]] const bool success = BarelyArpeggiator_SetGateRatio(*this, gate_ratio);
    assert(success);
  }

  /// Sets the instrument.
  ///
  /// @param instrument Optional instrument.
  void SetInstrument(std::optional<InstrumentHandle> instrument) noexcept {
    [[maybe_unused]] const bool success =
        BarelyArpeggiator_SetInstrument(*this, instrument ? *instrument : nullptr);
    assert(success);
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOff(double pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyArpeggiator_SetNoteOff(*this, pitch);
    assert(success);
  }

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch) noexcept {
    [[maybe_unused]] const bool success = BarelyArpeggiator_SetNoteOn(*this, pitch);
    assert(success);
  }

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept {
    [[maybe_unused]] const bool success = BarelyArpeggiator_SetRate(*this, rate);
    assert(success);
  }

  /// Sets the style.
  ///
  /// @param style Arpeggiator style.
  void SetStyle(ArpeggiatorStyle style) noexcept {
    [[maybe_unused]] const bool success =
        BarelyArpeggiator_SetStyle(*this, static_cast<BarelyArpeggiatorStyle>(style));
    assert(success);
  }
};

/// Arpeggiator handle.
class RepeaterHandle : public HandleWrapper<BarelyRepeaterHandle> {
 public:
  /// Creates a new `RepeaterHandle`.
  ///
  /// @param musician Musician handle.
  /// @param process_order Process order.
  /// @return Repeater handle.
  [[nodiscard]] static RepeaterHandle Create(MusicianHandle musician,
                                             int process_order = 0) noexcept {
    BarelyRepeaterHandle repeater = nullptr;
    [[maybe_unused]] const bool success =
        BarelyRepeater_Create(musician, static_cast<int32_t>(process_order), &repeater);
    assert(success);
    return RepeaterHandle(repeater);
  }

  /// Destroys a `RepeaterHandle`.
  ///
  /// @param repeater Repeater handle.
  static void Destroy(RepeaterHandle repeater) noexcept { BarelyRepeater_Destroy(repeater); }

  /// Default constructor.
  constexpr RepeaterHandle() noexcept = default;

  /// Creates a new `RepeaterHandle` from a raw handle.
  ///
  /// @param repeater Raw handle to repeater.
  explicit constexpr RepeaterHandle(BarelyRepeaterHandle repeater) noexcept
      : HandleWrapper(repeater) {}

  /// Clears all notes.
  void Clear() noexcept {
    [[maybe_unused]] const bool success = BarelyRepeater_Clear(*this);
    assert(success);
  }

  /// Returns whether the repeater is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept {
    bool is_playing = false;
    [[maybe_unused]] const bool success = BarelyRepeater_IsPlaying(*this, &is_playing);
    assert(success);
    return is_playing;
  }

  /// Pops the last note from the end.
  void Pop() noexcept {
    [[maybe_unused]] const bool success = BarelyRepeater_Pop(*this);
    assert(success);
  }

  /// Pushes a new note to the end.
  ///
  /// @param pitch_or Note pitch or silence.
  /// @param length Note length.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Push(std::optional<double> pitch_or, int length = 1) noexcept {
    [[maybe_unused]] const bool success = pitch_or ? BarelyRepeater_Push(*this, *pitch_or, length)
                                                   : BarelyRepeater_PushSilence(*this, length);
    assert(success);
  }

  /// Sets the instrument.
  ///
  /// @param instrument Optional instrument.
  void SetInstrument(std::optional<InstrumentHandle> instrument) noexcept {
    [[maybe_unused]] const bool success =
        BarelyRepeater_SetInstrument(*this, instrument ? *instrument : nullptr);
    assert(success);
  }

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept {
    [[maybe_unused]] const bool success = BarelyRepeater_SetRate(*this, rate);
    assert(success);
  }

  /// Sets the style.
  ///
  /// @param style Repeater style.
  void SetStyle(RepeaterStyle style) noexcept {
    [[maybe_unused]] const bool success =
        BarelyRepeater_SetStyle(*this, static_cast<BarelyRepeaterStyle>(style));
    assert(success);
  }

  /// Starts the repeater.
  ///
  /// @param pitch_offset Pitch offset.
  void Start(double pitch_offset = 0.0) noexcept {
    [[maybe_unused]] const bool success = BarelyRepeater_Start(*this, pitch_offset);
    assert(success);
  }

  /// Stop the repeater.
  void Stop() noexcept {
    [[maybe_unused]] const bool success = BarelyRepeater_Stop(*this);
    assert(success);
  }
};

/// A musical scale.
struct Scale : public BarelyScale {
 public:
  /// Default constructor.
  constexpr Scale() noexcept = default;

  /// Constructs a new `Scale`.
  ///
  /// @param pitches Span of pitches.
  /// @param root_pitch Root pitch.
  /// @param mode Mode.
  constexpr Scale(std::span<const double> pitches, double root_pitch = 0.0, int mode = 0) noexcept
      : Scale(BarelyScale{pitches.data(), static_cast<int32_t>(pitches.size()), root_pitch,
                          static_cast<int32_t>(mode)}) {}

  /// Constructs a new `Scale` from a raw type.
  ///
  /// @param scale Raw scale.
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr Scale(BarelyScale scale) noexcept : BarelyScale{scale} {
    assert(scale.pitches != nullptr);
    assert(scale.pitch_count > 0);
    assert(scale.mode >= 0 && scale.mode < scale.pitch_count);
  }

  /// Returns the pitch for a given degree.
  ///
  /// @param degree Degree.
  /// @return Pitch.
  [[nodiscard]] double GetPitch(int degree) const noexcept {
    double pitch = 0.0;
    [[maybe_unused]] const bool success = BarelyScale_GetPitch(this, degree, &pitch);
    assert(success);
    return pitch;
  }

  /// Returns the number of pitches in the scale.
  ///
  /// @return Number of pitches.
  [[nodiscard]] constexpr int GetPitchCount() const noexcept {
    return static_cast<int>(pitch_count);
  }
};

/// Scoped arpeggiator alias.
using Arpeggiator = ScopedHandleWrapper<ArpeggiatorHandle>;

/// Scoped repeater alias.
using Repeater = ScopedHandleWrapper<RepeaterHandle>;

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_BARELYCOMPOSER_H_
