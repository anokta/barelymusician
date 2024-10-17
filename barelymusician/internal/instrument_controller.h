#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/event.h"
#include "barelymusician/internal/instrument_processor.h"
#include "barelymusician/internal/message_queue.h"
#include "barelymusician/internal/sample_data.h"

namespace barely {

/// Class that controls an instrument.
class InstrumentController {
 public:
  /// Constructs a new `InstrumentController`.
  ///
  /// @param frame_rate Frame rate in hertz.
  /// @param reference_frequency Reference frequency in hertz.
  /// @param update_frame Update frame.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  InstrumentController(int frame_rate, double reference_frequency, int64_t update_frame) noexcept;

  /// Destroys `InstrumentController`.
  ~InstrumentController() noexcept;

  /// Non-copyable and non-movable.
  InstrumentController(const InstrumentController& other) noexcept = delete;
  InstrumentController& operator=(const InstrumentController& other) noexcept = delete;
  InstrumentController(InstrumentController&& other) noexcept = delete;
  InstrumentController& operator=(InstrumentController&& other) noexcept = delete;

  /// Returns a control value.
  ///
  /// @param index Control index.
  /// @return Pointer to control value, or nullptr if not found.
  [[nodiscard]] const double* GetControl(int index) const noexcept;
  [[nodiscard]] bool ResetControl(int index) noexcept;
  [[nodiscard]] bool SetControl(int index, double value) noexcept;

  /// Returns a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @return Pointer to note control value, or nullptr if not found.
  [[nodiscard]] const double* GetNoteControl(double pitch, int index) const noexcept;
  [[nodiscard]] bool ResetNoteControl(double pitch, int index) noexcept;
  [[nodiscard]] bool SetNoteControl(double pitch, int index, double value) noexcept;

  /// Returns whether a note is on or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if on, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept;

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
  /// @param pitch Note pitch.
  /// @return True if successful, false otherwise.
  bool ResetAllNoteControls(double pitch) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets the sample data.
  ///
  /// @param sample_data Sample data.
  void SetSampleData(SampleData sample_data) noexcept;

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
  /// @param update_frame Update frame.
  void Update(int64_t update_frame) noexcept;

 private:
  // Note off event alias.
  using NoteOffEvent = Event<NoteOffEventDefinition, double>;

  // Note on event alias.
  using NoteOnEvent = Event<NoteOnEventDefinition, double, double>;

  // Array of note control definitions.
  const std::vector<ControlDefinition> note_control_definitions_;

  // Array of controls.
  std::vector<Control> controls_;

  // Array of note control arrays by their pitches.
  std::unordered_map<double, std::vector<Control>> note_controls_;

  // Note off event.
  NoteOffEvent note_off_event_;

  // Note on event.
  NoteOnEvent note_on_event_;

  // Update frame.
  int64_t update_frame_ = 0;

  // Message queue.
  MessageQueue message_queue_;

  // Processor.
  InstrumentProcessor processor_;
};

}  // namespace barely

#endif  // BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
