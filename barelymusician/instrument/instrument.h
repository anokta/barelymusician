#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

#include <functional>
#include <unordered_set>
#include <vector>

#include "barelymusician/api/instrument.h"
#include "barelymusician/instrument/event.h"
#include "barelymusician/instrument/event_queue.h"
#include "barelymusician/instrument/parameter.h"

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
  [[nodiscard]] bool IsNoteOn(float pitch) const noexcept;

  /// Processes next output buffer.
  ///
  /// @param output Output buffer.
  /// @param num_output_channels Number of output channels.
  /// @param num_output_frames Number of output frames.
  /// @param timestamp Timestamp in seconds.
  void Process(float* output, int num_output_channels, int num_output_frames,
               double timestamp) noexcept;

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
  void SetData(barely::DataDefinition definition, double timestamp) noexcept;

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
  bool SetParameter(int index, double value, double timestamp) noexcept;

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
  // Controller that wraps main thread functionality.
  struct Controller {
    // Note off callback.
    std::function<void(float, double)> note_off_callback;

    // Note on callback.
    std::function<void(float, float, double)> note_on_callback;

    // List of parameters.
    std::vector<Parameter> parameters;

    // List of active note pitches.
    std::unordered_set<float> pitches;
  };

  // Processor that wraps audio thread functionality.
  struct Processor {
    // Destroy function.
    barely::InstrumentDefinition::DestroyCallback destroy_callback;

    // Process function.
    barely::InstrumentDefinition::ProcessCallback process_callback;

    // Set data function.
    barely::InstrumentDefinition::SetDataCallback set_data_callback;

    // Set note off function.
    barely::InstrumentDefinition::SetNoteOffCallback set_note_off_callback;

    // Set note on function.
    barely::InstrumentDefinition::SetNoteOnCallback set_note_on_callback;

    // List of parameter states.
    std::vector<barely::ParameterState> parameters;

    // Current state.
    barely::InstrumentState state;
  };

  // Controller.
  Controller controller_;

  // Event queue.
  EventQueue event_queue_;

  // Processor.
  Processor processor_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
