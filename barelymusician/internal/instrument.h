#ifndef BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_
#define BARELYMUSICIAN_INTERNAL_INSTRUMENT_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/internal/control.h"
#include "barelymusician/internal/event.h"
#include "barelymusician/internal/message_queue.h"
#include "barelymusician/internal/note.h"

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

  /// Creates a note.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @return Pointer to note.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  Note* CreateNote(double pitch, double intensity) noexcept;

  /// Destroys note.
  ///
  /// @param note Pointer to note.
  void DestroyNote(Note* note) noexcept;

  /// Returns a control value.
  ///
  /// @param control_id Control identifier.
  /// @return Pointer to control, or nullptr if not found.
  [[nodiscard]] Control* GetControl(int control_id) noexcept;

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

  /// Sets data.
  ///
  /// @param data Data.
  void SetData(std::vector<std::byte> data) noexcept;

  /// Updates the instrument.
  ///
  /// @param timestamp Timestamp in seconds.
  void Update(double timestamp) noexcept;

 private:
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

  // Array of note control definitions.
  const std::vector<ControlDefinition> note_control_definitions_;

  // Control map.
  ControlMap control_map_;

  // Map of pointers to notes.
  std::unordered_map<Note*, std::unique_ptr<Note>> notes_;

  // Note identifier counter.
  int note_id_counter_ = 0;

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
