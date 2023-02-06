#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/control.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/engine/number.h"
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
  /// @param frame_rate Frame rate in hertz.
  /// @param initial_tempo Initial tempo in beats per minute.
  /// @param initial_timestamp Initial timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(const InstrumentDefinition& definition, Integer frame_rate,
             double initial_tempo, double initial_timestamp) noexcept;

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
      Integer index) const noexcept;

  /// Returns note control.
  ///
  /// @param pitch Note pitch.
  /// @param index Control index.
  /// @return Reference to note control or error status.
  [[nodiscard]] StatusOr<std::reference_wrapper<const Control>> GetNoteControl(
      double pitch, Integer index) const noexcept;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept;

  /// Processes output samples.
  ///
  /// @param output_samples Interleaved array of output samples.
  /// @param output_channel_count Number of output channels.
  /// @param output_frame_count Number of output frames.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(double* output_samples, Integer output_channel_count,
               Integer output_frame_count, double timestamp) noexcept;

  /// Resets all controls to default value.
  void ResetAllControls() noexcept;

  /// Resets all note controls to default value.
  ///
  /// @param pitch Note pitch.
  /// @return Status.
  Status ResetAllNoteControls(double pitch) noexcept;

  /// Resets control to default value.
  ///
  /// @param index Control index.
  /// @return Status.
  Status ResetControl(Integer index) noexcept;

  /// Resets note control to default value.
  ///
  /// @param pitch Note pitch.
  /// @param index Control index.
  /// @return Status.
  Status ResetNoteControl(double pitch, Integer index) noexcept;

  /// Sets all notes off.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetAllNotesOff() noexcept;

  /// Sets control value.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_beat Control slope in value change per beat.
  /// @return Status.
  Status SetControl(Integer index, double value,
                    double slope_per_beat) noexcept;

  /// Sets control event callback.
  ///
  /// @param callback Control event callback.
  void SetControlEventCallback(ControlEventCallback callback) noexcept;

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Sets note control value.
  ///
  /// @param pitch Note pitch.
  /// @param index Note control index.
  /// @param value Note control value.
  /// @param slope_per_beat Note control slope in value change per beat.
  /// @return Status.
  Status SetNoteControl(double pitch, Integer index, double value,
                        double slope_per_beat) noexcept;

  /// Sets note control event callback.
  ///
  /// @param callback Note control event callback.
  void SetNoteControlEventCallback(NoteControlEventCallback callback) noexcept;

  /// Sets note off.
  ///
  /// @param pitch Note pitch.
  void SetNoteOff(double pitch) noexcept;

  /// Sets note off event callback.
  ///
  /// @param callback Note off event callback.
  void SetNoteOffEventCallback(NoteOffEventCallback callback) noexcept;

  /// Sets note on.
  ///
  /// @param pitch Note pitch.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void SetNoteOn(double pitch) noexcept;

  /// Sets note on event callback.
  ///
  /// @param callback Note on event callback.
  void SetNoteOnEventCallback(NoteOnEventCallback callback) noexcept;

  /// Sets playbck tempo.
  ///
  /// @param tempo Tempo in beats per minute.
  void SetTempo(double tempo) noexcept;

  /// Updates instrument at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
  // Returns corresponding slope per frame for a given `slope_per_beat`.
  double GetSlopePerFrame(double slope_per_beat) const noexcept;

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
  const Integer frame_rate_;

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
