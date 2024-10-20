#ifndef BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
#define BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// Arpeggiator handle alias.
typedef struct BarelyArpeggiator* BarelyArpeggiatorHandle;

/// Arpeggiator style enum alias.
typedef int32_t BarelyArpeggiatorStyle;

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

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
#include <optional>
#include <vector>

#include "barelymusician/common/random.h"

namespace barely {

/// Arpeggiator style.
enum class ArpeggiatorStyle : BarelyArpeggiatorStyle {
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
  /// @param musician Musician handle.
  /// @param process_order Process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  explicit Arpeggiator(MusicianHandle musician, int process_order = -1) noexcept;

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
  /// @param pitch Note pitch.
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
  void SetInstrument(std::optional<InstrumentHandle> instrument) noexcept;

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

  // Musician.
  MusicianHandle musician_;

  // Performer.
  PerformerHandle performer_;

  // Instrument.
  std::optional<InstrumentHandle> instrument_ = std::nullopt;

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
};

}  // namespace barely
#endif  // __cplusplus

#endif  // BARELYMUSICIAN_COMPONENTS_ARPEGGIATOR_H_
