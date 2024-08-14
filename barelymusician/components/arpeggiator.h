#ifndef BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
#define BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Arpeggiator alias.
typedef struct BarelyArpeggiator BarelyArpeggiator;

/// Arpeggiator style enum alias.
typedef int32_t BarelyArpeggiatorStyle;

/// Creates a new arpeggiator.
///
/// @param musician Pointer to musician.
/// @param process_order Arpeggiator process order.
/// @param out_arpeggiator Output pointer to arpeggiator.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_Create(BarelyMusician* musician, int32_t process_order,
                                            BarelyArpeggiator** out_arpeggiator);

/// Destroys an arpeggiator.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_Destroy(BarelyArpeggiator* arpeggiator);

/// Gets whether an arpeggiator note is on or not.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @param pitch Note pitch.
/// @param out_is_note_on Output true if on, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_IsNoteOn(const BarelyArpeggiator* arpeggiator, double pitch,
                                              bool* out_is_note_on);

/// Gets whether an arpeggiator is playing or not.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @param out_is_playing Output true if playing, false otherwise.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_IsPlaying(const BarelyArpeggiator* arpeggiator,
                                               bool* out_is_playing);

/// Sets all arpeggiator notes off.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetAllNotesOff(BarelyArpeggiator* arpeggiator);

/// Sets the gate ratio of an arpeggiator.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @param gate_ratio Gate ratio.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetGateRatio(BarelyArpeggiator* arpeggiator,
                                                  double gate_ratio);

/// Sets the instrument of an arpeggiator.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @param instrument Pointer to instrument.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetInstrument(BarelyArpeggiator* arpeggiator,
                                                   BarelyInstrument* instrument);

/// Sets an arpeggiator note off.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetNoteOff(BarelyArpeggiator* arpeggiator, double pitch);

/// Sets an arpeggiator note on.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetNoteOn(BarelyArpeggiator* arpeggiator, double pitch);

/// Sets the rate of an arpeggiator.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @param rate Rate in notes per beat.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetRate(BarelyArpeggiator* arpeggiator, double rate);

/// Sets the style of an arpeggiator.
///
/// @param arpeggiator Pointer to arpeggiator.
/// @param style Arpeggiator style.
/// @return True if successful, false otherwise.
BARELY_EXPORT bool BarelyArpeggiator_SetStyle(BarelyArpeggiator* arpeggiator,
                                              BarelyArpeggiatorStyle style);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
#include <optional>
#include <vector>

#include "barelymusician/common/random.h"

namespace barely {

/// Arpeggiator style.
enum class ArpeggiatorStyle {
  kUp = 0,
  kDown = 1,
  // kUpDown = 2,
  // kDownUp = 3,
  // kUpAndDown = 4,
  // kDownAndUp = 5,
  // kPinkyUp = 6,
  // kThumbUp = 7,
  kRandom = 8,
};

/// Simple arpeggiator that plays notes in sequence.
class Arpeggiator {
 public:
  // Constructs a new `Arpeggiator`.
  ///
  /// @param musician Musician pointer.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Arpeggiator(MusicianPtr musician, int process_order = -1) noexcept;

  /// Destroys `Arpeggiator`.
  ~Arpeggiator() noexcept;

  /// Non-copyable.
  Arpeggiator(const Arpeggiator& other) noexcept = delete;
  Arpeggiator& operator=(const Arpeggiator& other) noexcept = delete;

  /// Movable.
  Arpeggiator(Arpeggiator&& other) noexcept = default;
  Arpeggiator& operator=(Arpeggiator&& other) noexcept = default;

  /// Returns whether a note is on or not.
  ///
  /// @return True if on, false otherwise.
  bool IsNoteOn(double pitch) const noexcept;

  /// Returns whether the arpeggiator is playing or not.
  ///
  /// @return True if playing, false otherwise.
  bool IsPlaying() const noexcept;

  /// Sets all notes off.
  void SetAllNotesOff() noexcept;

  /// Sets the gate ratio.
  ///
  /// @param gate Gate ratio.
  void SetGateRatio(double gate_ratio) noexcept;

  /// Sets the instrument.
  ///
  /// @param instrument Optional instrument.
  void SetInstrument(std::optional<InstrumentPtr> instrument) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOff(double pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch) noexcept;

  /// Sets the rate.
  ///
  /// @param rate Rate in notes per beat.
  void SetRate(double rate) noexcept;

  /// Sets the style.
  ///
  /// @param style Arpeggiator style.
  void SetStyle(ArpeggiatorStyle style) noexcept;

 private:
  // Stop the arpeggiator.
  void Stop() noexcept;

  // Updates the arpeggiator.
  void Update() noexcept;

  // Performer.
  Performer performer_;

  // Instrument.
  std::optional<InstrumentPtr> instrument_ = std::nullopt;

  // Array of pitches to play.
  std::vector<double> pitches_;

  // Gate ratio.
  double gate_ratio_ = 1.0;

  // Style.
  ArpeggiatorStyle style_ = ArpeggiatorStyle::kUp;

  // Current index.
  int index_ = -1;

  // Random number generator.
  Random random_;

  // Task.
  Task task_;
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
