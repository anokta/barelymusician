#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include <cstddef>
#include <unordered_set>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/control.h"
#include "barelymusician/engine/message_queue.h"

namespace barely::internal {

/// Class that wraps instrument.
class Instrument {
 public:
  /// Definition alias.
  using Definition = barely::InstrumentDefinition;

  /// Note off callback alias.
  using NoteOffCallback = barely::Instrument::NoteOffCallback;

  /// Note on callback alias.
  using NoteOnCallback = barely::Instrument::NoteOnCallback;

  /// Constructs new `Instrument`.
  ///
  /// @param definition Instrument definition.
  /// @param frame_rate Frame rate in hz.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Instrument(const Definition& definition, int frame_rate) noexcept;

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
  /// @return Pointer to control.
  [[nodiscard]] const Control* GetControl(int index) const noexcept;

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

  /// Resets control to default value at timestamp.
  ///
  /// @param index Control index.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ResetControl(int index, double timestamp) noexcept;

  /// Sets control value at timestamp.
  ///
  /// @param index Control index.
  /// @param value Control value.
  /// @param slope_per_second Control slope in value change per second.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetControl(int index, double value, double slope_per_second,
                  double timestamp) noexcept;

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
  /// @return True if successful, false otherwise.
  bool SetNoteControl(double pitch, int index, double value,
                      double slope_per_second, double timestamp) noexcept;

  /// Sets note off callback.
  ///
  /// @param callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback callback) noexcept;

  /// Sets note on callback.
  ///
  /// @param callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback callback) noexcept;

  /// Starts note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void StartNote(double pitch, double timestamp) noexcept;

  /// Stops all notes at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void StopAllNotes(double timestamp) noexcept;

  /// Stops note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  void StopNote(double pitch, double timestamp) noexcept;

 private:
  // Returns corresponding frames for given `seconds`.
  int GetFrames(double seconds) const noexcept;

  // Returns corresponding seconds for given `frames`.
  double GetSeconds(int frames) const noexcept;

  // Returns corresponding slope per frame for a given `slope_per_second`.
  double GetSlopePerFrame(double slope_per_second) const noexcept;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;

  // List of controls.
  std::vector<Control> controls_;

  // List of note controls.
  std::vector<Control> note_controls_;

  // List of active note pitches.
  std::unordered_set<double> pitches_;

  // Destroy callback.
  Definition::DestroyCallback destroy_callback_;

  // Process callback.
  Definition::ProcessCallback process_callback_;

  // Set control callback.
  Definition::SetControlCallback set_control_callback_;

  // Set data callback.
  Definition::SetDataCallback set_data_callback_;

  // Set note control callback.
  Definition::SetNoteControlCallback set_note_control_callback_;

  // Set note off callback.
  Definition::SetNoteOffCallback set_note_off_callback_;

  // Set note on callback.
  Definition::SetNoteOnCallback set_note_on_callback_;

  // Sampling rate in hz.
  int frame_rate_;

  // State.
  void* state_ = nullptr;

  // Data.
  std::vector<std::byte> data_;

  // Message queue.
  MessageQueue message_queue_;
};

}  // namespace barely::internal

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
