#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/control.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/engine/status.h"

namespace barely::internal {

/// Instrument definition alias.
using InstrumentDefinition = barely::InstrumentDefinition;

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Control event callback alias.
  using ControlEventCallback = barely::Instrument::ControlEventCallback;

  /// Note control event callback alias.
  using NoteControlEventCallback = barely::Instrument::NoteControlEventCallback;

  /// Note off event callback alias.
  using NoteOffEventCallback = barely::Instrument::NoteOffEventCallback;

  /// Note on event callback alias.
  using NoteOnEventCallback = barely::Instrument::NoteOnEventCallback;

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

  /// Returns a control value.
  ///
  /// @param index Control index.
  /// @return Control value or error status.
  [[nodiscard]] StatusOr<double> GetControl(int index) const noexcept;

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
  /// @param callback Control event callback.
  void SetControlEventCallback(ControlEventCallback callback) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Sets a note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_beat Note control slope in value change per beat.
  /// @return Status.
  Status SetNoteControl(double pitch, int index, double value,
                        double slope_per_beat) noexcept;

  /// Sets the a note control event callback.
  ///
  /// @param callback Note control event callback.
  void SetNoteControlEventCallback(NoteControlEventCallback callback) noexcept;

  /// Sets a note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets the note off event callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOffEventCallback(NoteOffEventCallback callback) noexcept;

  /// Sets a note on.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch, double intensity) noexcept;

  /// Sets the note on event callback.
  ///
  /// @param callback Note on event callback.
  void SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept;

  /// Sets the tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates the instrument at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Returns the corresponding slope per frame for a given `slope_per_beat`.
  [[nodiscard]] double GetSlopePerFrame(double slope_per_beat) const noexcept;

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

  // Map of current note controls by note pitches.
  std::unordered_map<double, std::vector<Control>> note_controls_;

  // Control event callback.
  ControlEventCallback control_event_callback_;

  // Note control event callback.
  NoteControlEventCallback note_control_event_callback_;

  // Note off event callback.
  NoteOffEventCallback note_off_event_callback_;

  // Note on event callback.
  NoteOnEventCallback note_on_event_callback_;

  // Tempo in beats per minute.
  double tempo_ = 120.0;

  // Timestamp in seconds.
  double timestamp_ = 0;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
