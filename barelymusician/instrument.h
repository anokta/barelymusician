#ifndef BARELYMUSICIAN_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_H_

#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

#include "barelymusician/barelymusician.h"
#include "barelymusician/instrument_event.h"
#include "barelymusician/parameter.h"
#include "barelymusician/task_runner.h"

namespace barelyapi {

/// Class that wraps an instrument.
class Instrument {
 public:
  /// Constructs new `Instrument`.
  ///
  /// @param definition Instrument definition.
  /// @param sample_rate Sampling rate in hz.
  Instrument(const BarelyInstrumentDefinition& definition,
             int sample_rate) noexcept;

  /// Destroys `Instrument`.
  ~Instrument();

  /// Non-copyable and non-movable.
  Instrument(const Instrument& other) = delete;
  Instrument& operator=(const Instrument& other) = delete;
  Instrument(Instrument&& other) noexcept = delete;
  Instrument& operator=(Instrument&& other) noexcept = delete;

  /// Returns gain.
  ///
  /// @return gain Gain in amplitude.
  [[nodiscard]] float GetGain() const noexcept;

  /// Returns parameter.
  ///
  /// @param index Parameter index.
  /// @return Pointer to parameter.
  [[nodiscard]] const Parameter* GetParameter(int index) const noexcept;

  /// Returns whether instrument is muted or not.
  ///
  /// @return True if muted, false otherwise.
  [[nodiscard]] bool IsMuted() const noexcept;

  /// Returns whether note is active or not.
  ///
  /// @param pitch Note pitch.
  /// @return True if active, false otherwise.
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept;

  /// Processes next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output, int num_output_channels, int num_output_frames,
               double timestamp) noexcept;

  /// Processes event at timestamp.
  ///
  /// @param event Instrument event.
  /// @param timestamp Timestamp in seconds.
  void ProcessEvent(const InstrumentEvent& event, double timestamp) noexcept;

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
  /// @param data Data.
  /// @param timestamp Timestamp in seconds.
  void SetData(void* data, double timestamp) noexcept;

  /// Sets gain at timestamp.
  ///
  /// @param gain Gain in amplitude.
  /// @param timestamp Timestamp in seconds.
  void SetGain(float gain, double timestamp) noexcept;

  /// Sets whether instrument should be muted or not at timestamp.
  ///
  /// @param is_muted True if muted, false otherwise.
  /// @param timestamp Timestamp in seconds.
  void SetMuted(bool is_muted, double timestamp) noexcept;

  /// Sets note off callback.
  ///
  /// @param note_off_callback Note off callback.
  /// @param user_data User data.
  void SetNoteOffCallback(BarelyInstrument_NoteOffCallback note_off_callback,
                          void* user_data) noexcept;

  /// Sets note on callback.
  ///
  /// @param note_on_callback Note on callback.
  /// @param user_data User data.
  void SetNoteOnCallback(BarelyInstrument_NoteOnCallback note_on_callback,
                         void* user_data) noexcept;

  /// Sets parameter value at timestamp.
  ///
  /// @param index Parameter index.
  /// @param value Parameter value.
  /// @param timestamp Timestamp in seconds.
  /// @return True if successful, false otherwise.
  bool SetParameter(int index, float value, double timestamp) noexcept;

  /// Starts note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param intensity Note intensity.
  /// @param timestamp Timestamp in seconds.
  void StartNote(float pitch, float intensity, double timestamp) noexcept;

  /// Stops all notes at timestamp.
  ///
  /// @param timestamp Timestamp in seconds.
  void StopAllNotes(double timestamp) noexcept;

  /// Stops note at timestamp.
  ///
  /// @param pitch Note pitch.
  /// @param timestamp Timestamp in seconds.
  void StopNote(float pitch, double timestamp) noexcept;

 private:
  // Returns corresponding number of samples for given number of `seconds`.
  int GetSamples(double seconds) const noexcept;

  // Returns corresponding number of seconds for given number of `samples`.
  double GetSeconds(int samples) const noexcept;

  // Map of events by their timestamps.
  // TODO(#60): Turn this into a fixed size ring buffer.
  std::vector<std::pair<double, InstrumentEvent>> events_;
  TaskRunner runner_;

  // Gain in amplitude.
  float gain_ = 1.0f;

  // Denotes whether instrument is muted or not.
  bool is_muted_ = false;

  // Note off callback.
  std::function<void(float, double)> note_off_callback_;

  // Note on callback.
  std::function<void(float, float, double)> note_on_callback_;

  // List of parameters.
  std::vector<Parameter> parameters_;

  // List of active note pitches.
  std::unordered_set<float> pitches_;

  // Create function.
  BarelyInstrumentDefinition_CreateCallback create_callback_;

  // Destroy function.
  BarelyInstrumentDefinition_DestroyCallback destroy_callback_;

  // Process function.
  BarelyInstrumentDefinition_ProcessCallback process_callback_;

  // Set data function.
  BarelyInstrumentDefinition_SetDataCallback set_data_callback_;

  // Set note off function.
  BarelyInstrumentDefinition_SetNoteOffCallback set_note_off_callback_;

  // Set note on function.
  BarelyInstrumentDefinition_SetNoteOnCallback set_note_on_callback_;

  // Set parameter function.
  BarelyInstrumentDefinition_SetParameterCallback set_parameter_callback_;

  // Gain in amplitude.
  // TODO(#88): Turn this into `GainProcessor`.
  float processor_gain_ = 1.0f;

  // Sampling rate in hz.
  int sample_rate_ = 0;

  // State.
  void* state_ = nullptr;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_H_
