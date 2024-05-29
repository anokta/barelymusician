#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
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
  /// @param initial_timestamp Initial timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(const InstrumentDefinition& definition, int frame_rate,
             double initial_timestamp) noexcept;

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
  [[nodiscard]] const Control* GetControl(int id) const noexcept;

  /// Returns the frame rate.
  ///
  /// @return Frame rate in hz.
  [[nodiscard]] int GetFrameRate() const noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @return Pointer to note control, or nullptr if not found.
  [[nodiscard]] const Control* GetNoteControl(double pitch, int id) const noexcept;

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

  /// Resets all note control values.
  ///
  /// @param pitch Note pitch.
  /// @return True if successful, false otherwise.
  bool ResetAllNoteControls(double pitch) noexcept;

  /// Resets a control value.
  ///
  /// @param id Control identifier.
  /// @return True if successful, false otherwise.
  bool ResetControl(int id) noexcept;

  /// Resets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @return True if successful, false otherwise.
  bool ResetNoteControl(double pitch, int id) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets a control value.
  ///
  /// @param id Control identifier.
  /// @param value Control value.
  /// @return True if successful, false otherwise.
  bool SetControl(int id, double value) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param id Note control identifier.
  /// @param value Note control value.
  /// @return True if successful, false otherwise.
  bool SetNoteControl(double pitch, int id, double value) noexcept;

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

  /// Updates the instrument.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
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

  // Frame rate in hertz.
  const int frame_rate_;

  // Array of default note controls.
  const std::unordered_map<int, Control> default_note_controls_;

  // Map of controls by identifiers.
  std::unordered_map<int, Control> controls_;

  // Map of current note controls by note pitches.
  std::unordered_map<double, std::unordered_map<int, Control>> note_controls_;

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
