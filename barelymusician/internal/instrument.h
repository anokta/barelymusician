#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <cstddef>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/effect.h"
#include "barelymusician/internal/event.h"
#include "barelymusician/internal/id.h"
#include "barelymusician/internal/message_queue.h"
#include "barelymusician/internal/mutable_data.h"
#include "barelymusician/internal/status.h"

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
  Instrument(const InstrumentDefinition& definition, int frame_rate,
             double initial_tempo, double initial_timestamp) noexcept;

  /// Destroys `Instrument`.
  ~Instrument() noexcept;

  /// Non-copyable and non-movable.
  Instrument(const Instrument& other) noexcept = delete;
  Instrument& operator=(const Instrument& other) noexcept = delete;
  Instrument(Instrument&& other) noexcept = delete;
  Instrument& operator=(Instrument&& other) noexcept = delete;

  /// Creates a new effect.
  ///
  /// @param effect_id Effect identifier.
  /// @param definition Effect definition.
  /// @param process_order Effect process order.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void CreateEffect(Id effect_id, EffectDefinition definition,
                    int process_order) noexcept;

  /// Destroys an effect.
  ///
  /// @param effect_id Effect identifier.
  /// @return Status.
  Status DestroyEffect(Id effect_id) noexcept;

  /// Returns a control value.
  ///
  /// @param index Control index.
  /// @return Control value or error status.
  [[nodiscard]] StatusOr<double> GetControl(int index) const noexcept;

  /// Returns an effect control value.
  ///
  /// @param effect_id Effect identifier.
  /// @param index Effect control index.
  /// @return Effect control value or error status.
  [[nodiscard]] StatusOr<double> GetEffectControl(Id effect_id,
                                                  int index) const noexcept;

  /// Returns effect process order.
  ///
  /// @param effect_id Effect identifier.
  /// @return Process order or error status.
  [[nodiscard]] StatusOr<int> GetEffectProcessOrder(
      Id effect_id) const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return Note control value or error status.
  [[nodiscard]] StatusOr<double> GetNoteControl(double pitch,
                                                int index) const noexcept;

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
  /// @return Status.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Status Process(double* output_samples, int output_channel_count,
                 int output_frame_count, double timestamp) noexcept;

  /// Resets all control values.
  void ResetAllControls() noexcept;

  /// Resets all effect control values.
  ///
  /// @param effect_id Effect identifier.
  /// @return Status.
  Status ResetAllEffectControls(Id effect_id) noexcept;

  /// Resets all note control values.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status ResetAllNoteControls(double pitch) noexcept;

  /// Resets a control value.
  ///
  /// @param index Control index.
  /// @return Status.
  Status ResetControl(int index) noexcept;

  /// Resets a note control value.
  ///
  /// @param effect_id Effect identifier.
  /// @param index Effect control index.
  /// @return Status.
  Status ResetEffectControl(Id effect_id, int index) noexcept;

  /// Resets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return Status.
  Status ResetNoteControl(double pitch, int index) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return Status.
  Status SetControl(int index, double value, double slope_per_beat) noexcept;

  /// Sets the control event callback.
  ///
  /// @param callback Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition,
                       void* user_data) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Sets an effect control value.
  ///
  /// @param effect_id Effect identifier.
  /// @param index Effect control index.
  /// @param value Effect control value.
  /// @param slope_per_beat Effect control slope in value change per beat.
  /// @return Status.
  Status SetEffectControl(Id effect_id, int index, double value,
                          double slope_per_beat) noexcept;

  /// Sets the effect control event.
  ///
  /// @param effect_id Effect identifier.
  /// @param definition Effect control event definition.
  /// @param user_data Pointer to user data.
  /// @return Status.
  Status SetEffectControlEvent(Id effect_id, ControlEventDefinition definition,
                               void* user_data) noexcept;

  /// Sets effect data.
  ///
  /// @param effect_id Effect identifier.
  /// @param data Effect data.
  /// @return Status.
  Status SetEffectData(Id effect_id, std::vector<std::byte> data) noexcept;

  /// Sets effect process order.
  ///
  /// @param effect_id Effect identifier.
  /// @param process_order Effect process order.
  /// @return Status.
  Status SetEffectProcessOrder(Id effect_id, int process_order) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_beat Note control slope in value change per beat.
  /// @return Status.
  Status SetNoteControl(double pitch, int index, double value,
                        double slope_per_beat) noexcept;

  /// Sets the note control event.
  ///
  /// @param definition Note control event definition.
  /// @param user_data Pointer to user data.
  void SetNoteControlEvent(NoteControlEventDefinition definition,
                           void* user_data) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets the note off event.
  ///
  /// @param definition Note off event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOffEvent(NoteOffEventDefinition definition,
                       void* user_data) noexcept;

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
  void SetNoteOnEvent(NoteOnEventDefinition definition,
                      void* user_data) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the instrument at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Control event alias.
  using ControlEvent = Event<ControlEventDefinition, int, double>;

  // Note control event alias.
  using NoteControlEvent =
      Event<NoteControlEventDefinition, double, int, double>;

  // Note off event alias.
  using NoteOffEvent = Event<NoteOffEventDefinition, double>;

  // Note on event alias.
  using NoteOnEvent = Event<NoteOnEventDefinition, double, double>;

  // Effect info.
  struct EffectInfo {
    // Array of controls.
    std::vector<Control> controls;

    // Effect.
    std::unique_ptr<Effect> effect;

    // Process order.
    int process_order;

    // Control event.
    ControlEvent control_event;
  };

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

  // Map of effect infos by their identifiers.
  std::unordered_map<Id, EffectInfo> effect_infos_;

  // Ordered map of effects by their process order-identifier pairs.
  std::map<std::pair<int, Id>, Effect*> ordered_effects_;

  // Map of current note controls by note pitches.
  std::unordered_map<double, std::vector<Control>> note_controls_;

  // Control event.
  ControlEvent control_event_;

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

  // Array of effect identifier-reference pairs.
  MutableData<std::vector<std::pair<Id, Effect*>>> effect_id_ref_pairs_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
