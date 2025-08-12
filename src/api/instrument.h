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
  /// Control event callback alias.
  using ControlEventCallback = barely::Callback<BarelyControlEventCallback>;

  /// Note control event callback alias.
  using NoteControlEventCallback = barely::Callback<BarelyNoteControlEventCallback>;

  /// Note event callback alias.
  using NoteEventCallback = barely::Callback<BarelyNoteEventCallback>;

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
  /// @param output_samples Span of interleaved stereo output samples.
  /// @param process_frame Process frame.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(std::span<float> output_samples, int64_t process_frame) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param type Control type.
  /// @param value Control value.
  void SetControl(BarelyControlType type, float value) noexcept;

  /// Sets the control event callback.
  ///
  /// @param callback Control event callback.
  void SetControlEventCallback(ControlEventCallback callback) noexcept {
    control_event_callback_ = callback;
  }

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(float pitch, BarelyNoteControlType type, float value) noexcept;

  /// Sets the note control event callback.
  ///
  /// @param callback Note control event callback.
  void SetNoteControlEventCallback(NoteControlEventCallback callback) noexcept {
    note_control_event_callback_ = callback;
  }

  /// Sets the note event callback.
  ///
  /// @param callback Note event callback.
  void SetNoteEventCallback(NoteEventCallback callback) noexcept {
    note_event_callback_ = callback;
  }

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(float pitch) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param note_control_overrides Span of note control overrides.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(float pitch,
                 std::span<const BarelyNoteControlOverride> note_control_overrides) noexcept;

  /// Sets the sample data.
  ///
  /// @param slices Span of slices.
  void SetSampleData(std::span<const BarelySlice> slices) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_frame Update frame.
  void Update(int64_t update_frame) noexcept;

 private:
  // Engine.
  BarelyEngine& engine_;

  // Array of controls.
  barely::ControlArray controls_;

  // Control event callback.
  ControlEventCallback control_event_callback_ = {};

  // Map of note control arrays by their pitches.
  std::unordered_map<float, barely::NoteControlArray> note_controls_;

  // Note control event callback.
  NoteControlEventCallback note_control_event_callback_ = {};

  // Note event callback.
  NoteEventCallback note_event_callback_ = {};

  // Update frame.
  int64_t update_frame_ = 0;

  // Message queue.
  barely::MessageQueue message_queue_;

  // Instrument processor.
  barely::InstrumentProcessor processor_;
};

#endif  // BARELYMUSICIAN_API_INSTRUMENT_H_
