#ifndef BARELYMUSICIAN_PRIVATE_INSTRUMENT_IMPL_H_
#define BARELYMUSICIAN_PRIVATE_INSTRUMENT_IMPL_H_

#include <array>
#include <cstdint>
#include <span>
#include <unordered_map>

#include "barelymusician.h"
#include "common/callback.h"
#include "common/rng.h"
#include "dsp/control.h"
#include "dsp/instrument_processor.h"
#include "dsp/message_queue.h"
#include "dsp/sample_data.h"

namespace barely {

/// Class that implements an instrument.
class InstrumentImpl {
 public:
  /// Note callback alias.
  using NoteCallback = Callback<BarelyInstrument_NoteCallback>;

  /// Constructs a new `InstrumentImpl`.
  ///
  /// @param control_overrides Span of control overrides.
  /// @param rng Random number generator.
  /// @param sample_rate Sampling rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  /// @param update_sample Update sample.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentImpl(std::span<const ControlOverride> control_overrides, AudioRng& rng, int sample_rate,
                 float reference_frequency, int64_t update_sample) noexcept;

  /// Destroys `InstrumentImpl`.
  ~InstrumentImpl() noexcept;

  /// Non-copyable and non-movable.
  InstrumentImpl(const InstrumentImpl& other) noexcept = delete;
  InstrumentImpl& operator=(const InstrumentImpl& other) noexcept = delete;
  InstrumentImpl(InstrumentImpl&& other) noexcept = delete;
  InstrumentImpl& operator=(InstrumentImpl&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param type Control type.
  /// @return Control value.
  [[nodiscard]] float GetControl(ControlType type) const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @return Note control value.
  [[nodiscard]] const float* GetNoteControl(float pitch, NoteControlType type) const noexcept;

  /// Returns sampling rate.
  ///
  /// @return Sampling rate in hertz.
  [[nodiscard]] int GetSampleRate() const noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Span of mono output samples.
  /// @param process_sample Process sample.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Process(std::span<float> output_samples, int64_t process_sample) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  void SetControl(ControlType type, float value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(float pitch, NoteControlType type, float value) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept;

  /// Sets the note off callback.
  ///
  /// @param callback Note off callback.
  void SetNoteOffCallback(NoteCallback callback) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param note_control_overrides Span of note control overrides.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(float pitch, std::span<const NoteControlOverride> note_control_overrides) noexcept;

  /// Sets the note on callback.
  ///
  /// @param callback Note on callback.
  void SetNoteOnCallback(NoteCallback callback) noexcept;

  /// Sets the reference frequency.
  ///
  /// @param reference_frequency Reference frequency in hertz.
  void SetReferenceFrequency(float reference_frequency) noexcept;

  /// Sets the sample data.
  ///
  /// @param sample_data Sample data.
  void SetSampleData(SampleData sample_data) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_sample Update sample.
  void Update(int64_t update_sample) noexcept;

 private:
  // Returns a control array with overrides.
  ControlArray BuildControlArray(std::span<const ControlOverride> control_overrides) const noexcept;

  // Returns a note control array with overrides.
  NoteControlArray BuildNoteControlArray(
      std::span<const NoteControlOverride> note_control_overrides) const noexcept;

  // Returns an array of note control values from a given note control array.
  std::array<float, BarelyNoteControlType_kCount> BuildNoteControls(
      const NoteControlArray& note_control_array) const noexcept;

  // Array of controls.
  ControlArray controls_;

  // Map of note control arrays by their pitches.
  std::unordered_map<float, NoteControlArray> note_controls_;

  // Note off callback.
  NoteCallback note_off_callback_ = {};

  // Note on callback.
  NoteCallback note_on_callback_ = {};

  // Sampling rate in hertz.
  int sample_rate_ = 0;

  // Update sample.
  int64_t update_sample_ = 0;

  // Message queue.
  MessageQueue message_queue_;

  // Processor.
  InstrumentProcessor processor_;

  // TODO(#126): Temp hack to allow destroying by handle.
 public:
  BarelyEngineHandle engine = nullptr;
};

}  // namespace barely

struct BarelyInstrument : public barely::InstrumentImpl {};
static_assert(sizeof(BarelyInstrument) == sizeof(barely::InstrumentImpl));

#endif  // BARELYMUSICIAN_PRIVATE_INSTRUMENT_IMPL_H_
