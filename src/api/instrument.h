#ifndef BARELYMUSICIAN_API_INSTRUMENT_H_
#define BARELYMUSICIAN_API_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include "api/performer.h"
#include "api/task.h"
#include "common/callback.h"
#include "dsp/control.h"
#include "dsp/instrument_processor.h"

/// Implementation an instrument.
struct BarelyInstrument {
 public:
  /// Note event callback alias.
  using NoteEventCallback = barely::Callback<BarelyNoteEventCallback>;

  /// Constructs a new `BarelyInstrument`.
  ///
  /// @param engine Engine.
  /// @param control_overrides Span of instrument control overrides.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyInstrument(BarelyEngine& engine,
                   std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept;

  /// Destroys `BarelyInstrument`.
  ~BarelyInstrument() noexcept;

  /// Non-copyable and non-movable.
  BarelyInstrument(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument& operator=(const BarelyInstrument& other) noexcept = delete;
  BarelyInstrument(BarelyInstrument&& other) noexcept = delete;
  BarelyInstrument& operator=(BarelyInstrument&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param type Instrument control type.
  /// @return Instrument control value.
  [[nodiscard]] float GetControl(BarelyInstrumentControlType type) const noexcept;

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

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param type Instrument control type.
  /// @param value Instrument control value.
  void SetControl(BarelyInstrumentControlType type, float value) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param type Note control type.
  /// @param value Note control value.
  void SetNoteControl(float pitch, BarelyNoteControlType type, float value) noexcept;

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

 private:
  // Processes a control value.
  void ProcessControl(barely::InstrumentControlType type, float value) noexcept;

  // Updates the arpeggiator.
  void UpdateArp() noexcept;

  // Engine.
  BarelyEngine& engine_;

  // Sampling interval in seconds.
  float sample_interval_ = 0.0f;

  // Array of controls.
  barely::InstrumentControlArray controls_;

  // Map of note control arrays by their pitches.
  std::unordered_map<float, barely::NoteControlArray> note_controls_;
  std::vector<float> pitches_;  // sorted

  // Note event callback.
  NoteEventCallback note_event_callback_ = {};

  // Arpeggiator.
  BarelyPerformer arp_;
  BarelyTask arp_task_;
  int arp_pitch_index_ = -1;
  std::optional<float> arp_pitch_ = std::nullopt;
};

#endif  // BARELYMUSICIAN_API_INSTRUMENT_H_
