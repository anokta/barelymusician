#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <cstddef>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/event.h"
#include "barelymusician/internal/message_queue.h"
#include "barelymusician/internal/mutable.h"
#include "barelymusician/internal/observable.h"

namespace barely::internal {

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Constructs a new `Instrument`.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @param initial_tempo Initial tempo in beats per minute.
  /// @param initial_timestamp Initial timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(const InstrumentDefinition& definition, int frame_rate, double initial_tempo,
             double initial_timestamp) noexcept;

  /// Destroys `Instrument`.
  ~Instrument() noexcept;

  /// Non-copyable and non-movable.
  Instrument(const Instrument& other) noexcept = delete;
  Instrument& operator=(const Instrument& other) noexcept = delete;
  Instrument(Instrument&& other) noexcept = delete;
  Instrument& operator=(Instrument&& other) noexcept = delete;

  /// Creates a new effect.
  ///
  /// @param definition Effect definition.
  /// @param process_order Effect process order.
  /// @return Effect.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Observable<Effect> CreateEffect(EffectDefinition definition, int process_order) noexcept;

  /// Destroys an effect.
  ///
  /// @param effect Effect.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void DestroyEffect(Effect& effect) noexcept;

  /// Returns a control value.
  ///
  /// @param index Control index.
  /// @return Pointer to control, or nullptr if not found.
  [[nodiscard]] const Control* GetControl(int index) const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return Pointer to note control, or nullptr if not found.
  [[nodiscard]] const Control* GetNoteControl(double pitch, int index) const noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept;

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Process(double* output_samples, int output_channel_count, int output_frame_count,
               double timestamp) noexcept;

  /// Resets all control values.
  void ResetAllControls() noexcept;

  /// Resets all effect control values.
  ///
  /// @param effect_id Effect.
  void ResetAllEffectControls(Effect& effect) noexcept;

  /// Resets all note control values.
  ///
  /// @param pitch Note pitch.
  /// @return True if successful, false otherwise.
  bool ResetAllNoteControls(double pitch) noexcept;

  /// Resets a control value.
  ///
  /// @param index Control index.
  /// @return True if successful, false otherwise.
  bool ResetControl(int index) noexcept;

  /// Resets a note control value.
  ///
  /// @param effect Effect.
  /// @param index Effect control index.
  /// @return True if successful, false otherwise.
  bool ResetEffectControl(Effect& effect, int index) noexcept;

  /// Resets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return True if successful, false otherwise.
  bool ResetNoteControl(double pitch, int index) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return True if successful, false otherwise.
  bool SetControl(int index, double value, double slope_per_beat) noexcept;

  /// Sets the control event callback.
  ///
  /// @param callback Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Sets an effect control value.
  ///
  /// @param effect Effect.
  /// @param index Effect control index.
  /// @param value Effect control value.
  /// @param slope_per_beat Effect control slope in value change per beat.
  /// @return True if successful, false otherwise.
  bool SetEffectControl(Effect& effect, int index, double value, double slope_per_beat) noexcept;

  /// Sets effect data.
  ///
  /// @param effect Effect.
  /// @param data Effect data.
  void SetEffectData(Effect& effect, std::vector<std::byte> data) noexcept;

  /// Sets effect process order.
  ///
  /// @param effect Effect.
  /// @param process_order Effect process order.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetEffectProcessOrder(Effect& effect, int process_order) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_beat Note control slope in value change per beat.
  /// @return True if successful, false otherwise.
  bool SetNoteControl(double pitch, int index, double value, double slope_per_beat) noexcept;

  /// Sets the note control event.
  ///
  /// @param definition Note control event definition.
  /// @param user_data Pointer to user data.
  void SetNoteControlEvent(NoteControlEventDefinition definition, void* user_data) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets the note off event.
  ///
  /// @param definition Note off event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch, double intensity) noexcept;

  /// Sets the note on event.
  ///
  /// @param definition Note on event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the instrument at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Note control event alias.
  using NoteControlEvent = Event<NoteControlEventDefinition, double, int, double>;

  // Note off event alias.
  using NoteOffEvent = Event<NoteOffEventDefinition, double>;

  // Note on event alias.
  using NoteOnEvent = Event<NoteOnEventDefinition, double, double>;

  // Returns the corresponding slope per frame for a given `slope_per_beat`.
  [[nodiscard]] double GetSlopePerFrame(double slope_per_beat) const noexcept;

  // Updates effect references.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void UpdateEffectReferences() noexcept;

  // Destroy callback.
  const InstrumentDefinition::DestroyCallback destroy_callback_;

  // Process callback.
  const InstrumentDefinition::ProcessCallback process_callback_;

  // Set control callback.
  const InstrumentDefinition::SetControlCallback set_control_callback_;

  // Set data callback.
  const InstrumentDefinition::SetDataCallback set_data_callback_;

  // Set note control callback.
  const InstrumentDefinition::SetNoteControlCallback set_note_control_callback_;

  // Set note off callback.
  const InstrumentDefinition::SetNoteOffCallback set_note_off_callback_;

  // Set note on callback.
  const InstrumentDefinition::SetNoteOnCallback set_note_on_callback_;

  // Frame rate in hertz.
  const int frame_rate_;

  // Array of default note controls.
  const std::vector<Control> default_note_controls_;

  // Array of controls.
  std::vector<Control> controls_;

  // Ordered set of effects.
  std::set<std::pair<int, Effect*>> effects_;

  // Map of current note controls by note pitches.
  std::unordered_map<double, std::vector<Control>> note_controls_;

  // Control event.
  Control::Event control_event_;

  // Note control event.
  NoteControlEvent note_control_event_;

  // Note off event.
  NoteOffEvent note_off_event_;

  // Note on event.
  NoteOnEvent note_on_event_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;

  // List of pointers to effects.
  Mutable<std::vector<Effect*>> effect_ptrs_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
