#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_

#include <functional>
#include <unordered_map>
#include <vector>

#include "barelymusician/base/status.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument_controller.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "barelymusician/instrument/instrument_processor.h"

namespace barelyapi {

// Instrument note off callback signature.
using InstrumentNoteOffCallback =
    std::function<void(int64 instrument_id, int64 timestamp, float note_pitch)>;

// Instrument note on callback signature.
using InstrumentNoteOnCallback =
    std::function<void(int64 instrument_id, int64 timestamp, float note_pitch,
                       float note_intensity)>;

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
  int64 Create(InstrumentDefinition definition,
               InstrumentParamDefinitions param_definitions, int64 timestamp);

  // Destroys instrument at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status Destroy(int64 instrument_id, int64 timestamp);

  // Returns all active instrument notes.
  //
  // @param instrument_id Instrument id.
  // @return List of active note pitches if successful, error status otherwise.
  StatusOr<std::vector<float>> GetAllNotes(int64 instrument_id) const;

  // Returns all instrument parameters.
  //
  // @param instrument_id Instrument id.
  // @return List of parameters if successful, error status otherwise.
  StatusOr<std::vector<std::pair<int, float>>> GetAllParams(
      int64 instrument_id) const;

  // Returns instrument parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @return Parameter value if successful, error status otherwise.
  StatusOr<float> GetParam(int64 instrument_id, int param_id) const;

  // Returns whether instrument note is active or not.
  //
  // @param instrument_id Instrument id.
  // @param note_pitch Note pitch.
  // @return True if note is active, error status otherwise.
  StatusOr<bool> IsNoteOn(int64 instrument_id, float note_pitch) const;

  // Processes the next instrument output buffer at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param output Pointer to the output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @return Status.
  Status Process(int64 instrument_id, int64 timestamp, float* output,
                 int num_channels, int num_frames);

  // Resets all instrument parameters to their default values at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status ResetAllParams(int64 instrument_id, int64 timestamp);

  // Resets instrument parameter to its default value at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param param_id Parameter id.
  // @return Status.
  Status ResetParam(int64 instrument_id, int64 timestamp, int param_id);

  // Stops all active instrument notes at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status SetAllNotesOff(int64 instrument_id, int64 timestamp);

  // Sets instrument custom data at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param custom_data Custom data.
  // @return Status.
  Status SetCustomData(int64 instrument_id, int64 timestamp, void* custom_data);

  // Sets instrument note off at timestamp.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param note_pitch Note pitch.
  // @return Status.
  Status SetNoteOff(int64 instrument_id, int64 timestamp, float note_pitch);

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
  Status SetNoteOn(int64 instrument_id, int64 timestamp, float note_pitch,
                   float note_intensity);

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
  Status SetParam(int64 instrument_id, int64 timestamp, int param_id,
                  float param_value);

 private:
  // Sets instrument processor |data| at |timestamp|.
  void SetProcessorData(int64 instrument_id, int64 timestamp,
                        InstrumentData data);

  // List of instruments.
  std::unordered_map<int64, InstrumentController> controllers_;
  std::unordered_map<int64, InstrumentProcessor> processors_;

  // Instrument note off callback.
  InstrumentNoteOffCallback note_off_callback_;

  // Instrument note on callback.
  InstrumentNoteOnCallback note_on_callback_;

  // Instrument id counter.
  int64 id_counter_;

  // Audio thread task runner.
  TaskRunner task_runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
