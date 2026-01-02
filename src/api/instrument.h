#ifndef BARELYMUSICIAN_API_INSTRUMENT_H_
#define BARELYMUSICIAN_API_INSTRUMENT_H_

#include <barelymusician.h>

#include <cstdint>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include "api/performer.h"
#include "common/callback.h"
#include "common/constants.h"
#include "dsp/control.h"
#include "engine/instrument_params.h"
#include "engine/task_state.h"

/// Implementation an instrument.
struct BarelyInstrument {
 public:
  /// Note event callback alias.
  using NoteEventCallback = barely::Callback<BarelyNoteEventCallback>;

  // Array of controls.
  barely::InstrumentControlArray controls_;

  /// Pointer to engine.
  BarelyEngine* engine_ = nullptr;

  /// Instrument index.
  uint32_t instrument_index = {};

  /// Arpeggiator index.
  BarelyPerformer arp;

  /// Arpeggiator task index.
  TaskState arp_task;

  barely::InstrumentParams params = {};

  /// Constructs a new `BarelyInstrument`.
  ///
  /// @param engine Engine.
  /// @param control_overrides Span of instrument control overrides.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  BarelyInstrument(BarelyEngine& engine,
                   std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept;
  // TODO(#126): Temp workaround.
  BarelyInstrument() noexcept {}
  void Init(BarelyEngine& engine,
            std::span<const BarelyInstrumentControlOverride> control_overrides) noexcept;

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

  // Map of note control arrays by their pitches.
  std::unordered_map<float, barely::NoteControlArray> note_controls_;
  std::vector<float> pitches_;  // sorted

  // Note event callback.
  NoteEventCallback note_event_callback_ = {};

  // Arpeggiator.
  std::optional<float> arp_pitch_ = std::nullopt;
  int arp_pitch_index_ = -1;
};

#endif  // BARELYMUSICIAN_API_INSTRUMENT_H_
