#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/base/status.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/instrument/instrument_controller.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "barelymusician/instrument/instrument_processor.h"

namespace barelyapi {

// Instrument note off callback signature.
using InstrumentNoteOffCallback = std::function<void(
    std::int64_t instrument_id, std::int64_t timestamp, float note_pitch)>;

// Instrument note on callback signature.
using InstrumentNoteOnCallback =
    std::function<void(std::int64_t instrument_id, std::int64_t timestamp,
                       float note_pitch, float note_intensity)>;

// Instrument manager.
class InstrumentManager {
 public:
  // Constructs new |InstrumentManager|.
  InstrumentManager();

  // Creates new instrument at timestamp.
  //
  // @param definition Instrument definition.
  // @param param_definitions Instrument parameter definitions.
  // @param timestamp Timestamp in frames.
  // @return Instrument id.
  std::int64_t Create(InstrumentDefinition definition,
                      InstrumentParamDefinitions param_definitions,
                      std::int64_t timestamp);

  // Destroys instrument at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status Destroy(std::int64_t instrument_id, std::int64_t timestamp);

  // Returns all active instrument notes.
  //
  // @param instrument_id Instrument id.
  // @return List of active note pitches if successful, error status otherwise.
  StatusOr<std::vector<float>> GetAllNotes(std::int64_t instrument_id) const;

  // Returns all instrument parameters.
  //
  // @param instrument_id Instrument id.
  // @return List of parameters if successful, error status otherwise.
  StatusOr<std::vector<std::pair<int, float>>> GetAllParams(
      std::int64_t instrument_id) const;

  // Returns instrument parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @return Parameter value if successful, error status otherwise.
  StatusOr<float> GetParam(std::int64_t instrument_id, int param_id) const;

  // Returns whether instrument note is active or not.
  //
  // @param instrument_id Instrument id.
  // @param note_pitch Note pitch.
  // @return True if note is active, error status otherwise.
  StatusOr<bool> IsNoteOn(std::int64_t instrument_id, float note_pitch) const;

  // Processes the next instrument output buffer at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param output Pointer to the output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @return Status.
  Status Process(std::int64_t instrument_id, std::int64_t timestamp,
                 float* output, int num_channels, int num_frames);

  // Resets all parameters of all instruments to their default values at
  // timestamp.
  //
  // @param timestamp Timestamp in frames.
  // @return Status.
  void ResetAllParams(std::int64_t timestamp);

  // Resets all instrument parameters to their default values at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status ResetAllParams(std::int64_t instrument_id, std::int64_t timestamp);

  // Resets instrument parameter to its default value at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param param_id Parameter id.
  // @return Status.
  Status ResetParam(std::int64_t instrument_id, std::int64_t timestamp,
                    int param_id);

  // Sets all active notes of all instruments off at timestamp.
  //
  // @param timestamp Timestamp in frames.
  // @return Status.
  void SetAllNotesOff(std::int64_t timestamp);

  // Sets all active instrument notes off at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status SetAllNotesOff(std::int64_t instrument_id, std::int64_t timestamp);

  // Sets instrument custom data at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param custom_data Custom data.
  // @return Status.
  Status SetCustomData(std::int64_t instrument_id, std::int64_t timestamp,
                       void* custom_data);

  // Sets instrument note off at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param note_pitch Note pitch.
  // @return Status.
  Status SetNoteOff(std::int64_t instrument_id, std::int64_t timestamp,
                    float note_pitch);

  // Sets instrument note off callback.
  //
  // @param note_off_callback Instrument note off callback.
  void SetNoteOffCallback(InstrumentNoteOffCallback note_off_callback);

  // Sets instrument note on at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param note_pitch Note pitch.
  // @param note_intensity Note intensity.
  // @return Status.
  Status SetNoteOn(std::int64_t instrument_id, std::int64_t timestamp,
                   float note_pitch, float note_intensity);

  // Sets instrument note on callback.
  //
  // @param note_on_callback Instrument note on callback.
  void SetNoteOnCallback(InstrumentNoteOnCallback note_on_callback);

  // Sets instrument parameter value at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param param_id Parameter id.
  // @param param_value Parameter value.
  // @return Status.
  Status SetParam(std::int64_t instrument_id, std::int64_t timestamp,
                  int param_id, float param_value);

 private:
  // Sets instrument processor |data| at |timestamp|.
  void SetProcessorData(std::int64_t instrument_id, std::int64_t timestamp,
                        InstrumentData data);

  // List of instruments.
  std::unordered_map<std::int64_t, InstrumentController> controllers_;
  std::unordered_map<std::int64_t, InstrumentProcessor> processors_;

  // Instrument note off callback.
  InstrumentNoteOffCallback note_off_callback_;

  // Instrument note on callback.
  InstrumentNoteOnCallback note_on_callback_;

  // Audio thread task runner.
  TaskRunner task_runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
