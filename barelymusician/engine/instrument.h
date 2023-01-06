#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/control.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/engine/status.h"

namespace barely::internal {

/// Instrument definition alias.
using InstrumentDefinition = barely::InstrumentDefinition;

/// Class that wraps instrument.
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

  /// Constructs new `Instrument`.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(const InstrumentDefinition& definition, int frame_rate) noexcept;

  /// Destroys `Instrument`.
  ~Instrument() noexcept;

  /// Non-copyable and non-movable.
  Instrument(const Instrument& other) = delete;
  Instrument& operator=(const Instrument& other) = delete;
  Instrument(Instrument&& other) noexcept = delete;
  Instrument& operator=(Instrument&& other) noexcept = delete;

  /// Returns control.
  ///
  /// @param index Control index.
  /// @return Reference to control or error status.
  [[nodiscard]] StatusOr<std::reference_wrapper<const Control>> GetControl(
      int index) const noexcept;

  /// Returns note control.
  ///
  /// @param pitch Note pitch.
  /// @param index Control index.
  /// @return Reference to note control or error status.
  [[nodiscard]] StatusOr<std::reference_wrapper<const Control>> GetNoteControl(
      double pitch, int index) const noexcept;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept;

  /// Processes output samples at timestamp.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(double* output_samples, int output_channel_count,
               int output_frame_count, double timestamp) noexcept;

  /// Resets all controls to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ResetAllControls(double timestamp) noexcept;

  /// Resets all note controls to default value at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ResetAllNoteControls(double pitch, double timestamp) noexcept;

  /// Resets control to default value at timestamp.
  ///
  /// @param index Control index.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ResetControl(int index, double timestamp) noexcept;

  /// Resets note control to default value at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param index Control index.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status ResetNoteControl(double pitch, int index, double timestamp) noexcept;

  /// Sets all notes off at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff(double timestamp) noexcept;

  /// Sets control value at timestamp.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_second Control slope in value change per second.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetControl(int index, double value, double slope_per_second,
                    double timestamp) noexcept;

  /// Sets control event callback.
  ///
  /// @param callback Control event callback.
  void SetControlEventCallback(ControlEventCallback callback) noexcept;

  /// Sets data at timestamp.
  ///
  /// @param data Data.
  /// @param timestamp Timestamp in seconds.
  void SetData(std::vector<std::byte> data, double timestamp) noexcept;

  /// Sets note control value at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_second Note control slope in value change per second.
  /// @param timestamp Timestamp in seconds.
  /// @return Status.
  Status SetNoteControl(double pitch, int index, double value,
                        double slope_per_second, double timestamp) noexcept;

  /// Sets note control event callback.
  ///
  /// @param callback Note control event callback.
  void SetNoteControlEventCallback(NoteControlEventCallback callback) noexcept;

  /// Sets note off at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  void SetNoteOff(double pitch, double timestamp) noexcept;

  /// Sets note off event callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOffEventCallback(NoteOffEventCallback callback) noexcept;

  /// Sets note on at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch, double timestamp) noexcept;

  /// Sets note on event callback.
  ///
  /// @param callback Note on event callback.
  void SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept;

  // TODO(#109): Needs an update mechanism for control values with slope.

 private:
  // Returns corresponding frames for a given number of `seconds`.
  int GetFrames(double seconds) const noexcept;

  // Returns corresponding seconds for a given number of `frames`.
  double GetSeconds(int frames) const noexcept;

  // Returns corresponding slope per frame for a given `slope_per_second`.
  double GetSlopePerFrame(double slope_per_second) const noexcept;

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

  // Sampling rate in hz.
  const int frame_rate_;

  // List of default note controls.
  const std::vector<Control> default_note_controls_;

  // List of controls.
  std::vector<Control> controls_;

  // Map of active note controls by note pitches.
  std::unordered_map<double, std::vector<Control>> note_controls_;

  // Control event callback.
  ControlEventCallback control_event_callback_;

  // Note control event callback.
  NoteControlEventCallback note_control_event_callback_;

  // Note off event callback.
  NoteOffEventCallback note_off_event_callback_;

  // Note on event callback.
  NoteOnEventCallback note_on_event_callback_;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
