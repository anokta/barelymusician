#ifndef BARELYMUSICIAN_API_INSTRUMENT_H_
#define BARELYMUSICIAN_API_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>
#include <span>
#include <unordered_map>

#include "common/callback.h"
#include "dsp/control.h"
#include "dsp/instrument_processor.h"
#include "dsp/message_queue.h"

/// Implementation an instrument.
struct BarelyInstrument {
 public:
  /// Note callback alias.
  using NoteCallback = barely::Callback<BarelyInstrument_NoteCallback>;

  /// Constructs a new `BarelyInstrument`.
  ///
  /// @param engine Engine.
  /// @param control_overrides Span of control overrides.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyInstrument(BarelyEngine& engine,
                   std::span<const BarelyControlOverride> control_overrides) noexcept;

  /// Destroys `BarelyInstrument`.
  ~BarelyInstrument() noexcept;

  /// Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param type Control type.
  /// @return Control value.
  [[nodiscard]] float GetControl(BarelyControlType type) const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @return Note control value.
  [[nodiscard]] const float* GetNoteControl(float pitch, BarelyNoteControlType type) const noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Span of mono output samples.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Process(std::span<float> output_samples, double timestamp) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  void SetControl(BarelyControlType type, float value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(float pitch, BarelyNoteControlType type, float value) noexcept;

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
  void SetNoteOn(float pitch,
                 std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept;

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
  /// @param slices Span of slices.
  void SetSampleData(std::span<const BarelySlice> slices) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_sample Update sample.
  void Update(int64_t update_sample) noexcept;

 private:
  // Pointer to engine.
  BarelyEngine* engine_ = nullptr;

  // Array of controls.
  barely::ControlArray controls_;

  // Map of note control arrays by their pitches.
  std::unordered_map<float, barely::NoteControlArray> note_controls_;

  // Note off callback.
  NoteCallback note_off_callback_ = {};

  // Note on callback.
  NoteCallback note_on_callback_ = {};

  // Update sample.
  int64_t update_sample_ = 0;

  // Message queue.
  barely::MessageQueue message_queue_;

  // Instrument processor.
  barely::InstrumentProcessor processor_;
};

#endif  // BARELYMUSICIAN_API_INSTRUMENT_H_
