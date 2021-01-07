#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_

#include <unordered_map>
#include <vector>

#include "barelymusician/base/constants.h"
#include "barelymusician/base/status.h"
#include "barelymusician/base/task_runner.h"
#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument_controller.h"
#include "barelymusician/instrument/instrument_data.h"
#include "barelymusician/instrument/instrument_definition.h"
#include "barelymusician/instrument/instrument_processor.h"

namespace barelyapi {

// Class that manages processing of instruments.
class InstrumentManager {
 public:
  // Constructs new |InstrumentManager|.
  InstrumentManager();

  // Creates new instrument.
  //
  // @param instrument instrument Instrument to play.
  // @param params Default instrument params.
  // @return Instrument id.
  StatusOr<int64> Create(InstrumentDefinition definition,
                         InstrumentParamDefinitions param_definitions);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  // @return Status.
  Status Destroy(int64 instrument_id);

  StatusOr<std::vector<float>> GetAllNotes(int64 instrument_id) const;

  StatusOr<std::vector<Param>> GetAllParams(int64 instrument_id) const;

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @return Parameter value, if found.
  StatusOr<float> GetParam(int64 instrument_id, int param_id) const;

  // Returns whether note is active or not.
  //
  // @param instrument_id Instrument id.
  // @param pitch Pitch.
  // @return True if note is active, if instrument found.
  StatusOr<bool> IsNoteOn(int64 instrument_id, float pitch) const;

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp Begin timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @return Status.
  Status Process(int64 instrument_id, int64 timestamp, float* output,
                 int num_channels, int num_frames);

  Status ResetParam(int64 instrument_id, int64 timestamp, int param_id);

  // Sets custom data.
  //
  // @param instrument_id Instrument id.
  // @param custom_data Custom data.
  // @return True if successful, if instrument found.
  Status SetCustomData(int64 instrument_id, int64 timestamp, void* custom_data);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param pitch Note pitch.
  // @return True if successful, if instrument found.
  Status SetNoteOff(int64 instrument_id, int64 timestamp, float pitch);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param pitch Note pitch.
  // @param intensity Note intensity.
  // @return True if successful, if instrument found.
  Status SetNoteOn(int64 instrument_id, int64 timestamp, float pitch,
                   float intensity);

  // Sets control parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @param param_value Parameter value.
  // @return True if successful, if instrument parameter found.
  Status SetParam(int64 instrument_id, int64 timestamp, int param_id,
                  float param_value);

 private:
  void SetProcessorData(int64 instrument_id, int64 timestamp,
                        InstrumentData data);

  // List of instruments.
  std::unordered_map<int64, InstrumentController> controllers_;
  std::unordered_map<int64, InstrumentProcessor> processors_;

  // int64 counter.
  int64 id_counter_;

  // Task runner.
  TaskRunner task_runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
