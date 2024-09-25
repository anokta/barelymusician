#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/event.h"
#include "barelymusician/internal/message_queue.h"

namespace barely::internal {

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Constructs a new `Instrument`.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hertz.
  /// @param update_frame Update frame.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(const InstrumentDefinition& definition, int frame_rate, int64_t update_frame) noexcept;

  /// Destroys `Instrument`.
  ~Instrument() noexcept;

  /// Non-copyable and non-movable.
  Instrument(const Instrument& other) noexcept = delete;
  Instrument& operator=(const Instrument& other) noexcept = delete;
  Instrument(Instrument&& other) noexcept = delete;
  Instrument& operator=(Instrument&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param id Control identifier.
  /// @return Pointer to control, or nullptr if not found.
  [[nodiscard]] Control* GetControl(int id) noexcept;
  [[nodiscard]] const Control* GetControl(int id) const noexcept;

  /// Returns a note control value.
  ///
  /// @param note Note value.
  /// @param id Note control identifier.
  /// @return Pointer to note control, or nullptr if not found.
  [[nodiscard]] Control* GetNoteControl(double note, int id) noexcept;
  [[nodiscard]] const Control* GetNoteControl(double note, int id) const noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param note Note value.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(double note) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Array of interleaved output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param process_frame Process frame.
  /// @return True if successful, false otherwise.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  bool Process(double* output_samples, int output_channel_count, int output_frame_count,
               int64_t process_frame) noexcept;

  /// Resets all control values.
  void ResetAllControls() noexcept;

  /// Resets all note control values.
  ///
  /// @param note Note value.
  /// @return True if successful, false otherwise.
  bool ResetAllNoteControls(double note) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets the control event callback.
  ///
  /// @param callback Control event definition.
  /// @param user_data Pointer to user data.
  void SetControlEvent(ControlEventDefinition definition, void* user_data) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Sets the note control event.
  ///
  /// @param definition Note control event definition.
  /// @param user_data Pointer to user data.
  void SetNoteControlEvent(NoteControlEventDefinition definition, void* user_data) noexcept;

  /// Sets a note off.
  ///
  /// @param note Note value.
  void SetNoteOff(double note) noexcept;

  /// Sets the note off event.
  ///
  /// @param definition Note off event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOffEvent(NoteOffEventDefinition definition, void* user_data) noexcept;

  /// Sets a note on.
  ///
  /// @param note Note value.
  /// @param intensity Note intensity.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double note, double intensity) noexcept;

  /// Sets the note on event.
  ///
  /// @param definition Note on event definition.
  /// @param user_data Pointer to user data.
  void SetNoteOnEvent(NoteOnEventDefinition definition, void* user_data) noexcept;

  /// Sets the tuning.
  ///
  /// @param definition Tuning definition.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetTuning(const TuningDefinition* definition) noexcept;

  /// Updates the instrument.
  ///
  /// @param update_frame Update frame.
  void Update(int64_t update_frame) noexcept;

 private:
  // Note control event alias.
  using NoteControlEvent = Event<NoteControlEventDefinition, double, int, double>;

  // Note off event alias.
  using NoteOffEvent = Event<NoteOffEventDefinition, double>;

  // Note on event alias.
  using NoteOnEvent = Event<NoteOnEventDefinition, double, double>;

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

  // Set tuning callback.
  const InstrumentDefinition::SetTuningCallback set_tuning_callback_;

  // Array of note control definitions.
  const std::vector<ControlDefinition> note_control_definitions_;

  // Control map.
  ControlMap control_map_;

  // Map of note control maps by their note values.
  std::unordered_map<double, ControlMap> note_control_maps_;

  // Control event.
  Control::Event control_event_;

  // Note control event.
  NoteControlEvent note_control_event_;

  // Note off event.
  NoteOffEvent note_off_event_;

  // Note on event.
  NoteOnEvent note_on_event_;

  // Update frame.
  int64_t update_frame_ = 0;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
