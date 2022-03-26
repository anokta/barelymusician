#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_H_

#include <unordered_set>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/engine/data.h"
#include "barelymusician/engine/event.h"
#include "barelymusician/engine/event_queue.h"
#include "barelymusician/engine/parameter.h"

namespace barelyapi {

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

  /// Returns parameter.
  ///
  /// @param index Parameter index.
  /// @return Pointer to parameter.
  [[nodiscard]] const Parameter* GetParameter(int index) const noexcept;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(double pitch) const noexcept;

  /// Processes next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void Process(double* output, int num_output_channels, int num_output_frames,
               double timestamp) noexcept;

  /// Processes event at timestamp.
  ///
  /// @param event Instrument event.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void ProcessEvent(Event event, double timestamp) noexcept;

  /// Resets all parameters to default value at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void ResetAllParameters(double timestamp) noexcept;

  /// Resets parameter to default value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool ResetParameter(int index, double timestamp) noexcept;

  /// Sets data at timestamp.
  ///
  /// @param definition Data definition.
  /// @param timestamp Timestamp in seconds.
  void SetData(Data::Definition definition, double timestamp) noexcept;

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback) noexcept;

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback) noexcept;

  /// Sets parameter value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @param slope Parameter slope in value change per second.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetParameter(int index, double value, double slope,
                    double timestamp) noexcept;

  /// Starts note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Timestamp in seconds.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  void StartNote(double pitch, double intensity, double timestamp) noexcept;

  /// Stops all notes at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
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

  // Returns corresponding slope per frame for a given `slope`.
  double GetSlopePerFrame(double slope) const noexcept;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;

  // List of parameters.
  std::vector<Parameter> parameters_;

  // List of active note pitches.
  std::unordered_set<double> pitches_;

  // Destroy callback.
  Definition::DestroyCallback destroy_callback_;

  // Process callback.
  Definition::ProcessCallback process_callback_;

  // Set data callback.
  Definition::SetDataCallback set_data_callback_;

  // Set note off callback.
  Definition::SetNoteOffCallback set_note_off_callback_;

  // Set note on callback.
  Definition::SetNoteOnCallback set_note_on_callback_;

  // Set parameter callback.
  Definition::SetParameterCallback set_parameter_callback_;

  // Sampling rate in hz.
  int frame_rate_;

  // State.
  void* state_ = nullptr;

  // Event queue.
  EventQueue event_queue_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_H_
