#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_

#include <functional>
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
  // Note off callback signature.
  using NoteOffCallback = std::function<void(
      int64 timestamp, int64 instrument_id, float note_pitch)>;

  // Note on callback signature.
  using NoteOnCallback =
      std::function<void(int64 timestamp, int64 instrument_id, float note_pitch,
                         float note_intensity)>;

  // Constructs new |InstrumentManager|.
  InstrumentManager();

  // Creates new instrument.
  //
  // @param instrument instrument Instrument to play.
  // @param params Default instrument params.
  // @param timestamp Timestamp in frames.
  // @return Instrument id.
  StatusOr<int64> Create(InstrumentDefinition definition,
                         InstrumentParamDefinitions param_definitions,
                         int64 timestamp);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return Status.
  Status Destroy(int64 instrument_id, int64 timestamp);

  StatusOr<std::vector<float>> GetAllNotes(int64 instrument_id) const;

  StatusOr<std::vector<std::pair<int, float>>> GetAllParams(
      int64 instrument_id) const;

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @return Parameter value, if found.
  StatusOr<float> GetParam(int64 instrument_id, int param_id) const;

  // Returns whether note is active or not.
  //
  // @param instrument_id Instrument id.
  // @param note_pitch Note pitch.
  // @return True if note is active, if instrument found.
  StatusOr<bool> IsNoteOn(int64 instrument_id, float note_pitch) const;

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @return Status.
  Status Process(int64 instrument_id, int64 timestamp, float* output,
                 int num_channels, int num_frames);

  // Resets all parameters to their default values.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return True if successful, if instrument found.
  Status ResetAllParams(int64 instrument_id, int64 timestamp);

  // Resets parameter to its default value.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param param_id Parameter id.
  // @return True if successful, if instrument parameter found.
  Status ResetParam(int64 instrument_id, int64 timestamp, int param_id);

  // Stops all playing notes.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @return True if successful, if instrument found.
  Status SetAllNotesOff(int64 instrument_id, int64 timestamp);

  // Sets custom data.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param custom_data Custom data.
  // @return True if successful, if instrument found.
  Status SetCustomData(int64 instrument_id, int64 timestamp, void* custom_data);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param note_pitch Note pitch.
  // @return True if successful, if instrument found.
  Status SetNoteOff(int64 instrument_id, int64 timestamp, float note_pitch);

  // Sets note off callback.
  //
  // @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback note_off_callback);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param note_pitch Note pitch.
  // @param note_intensity Note intensity.
  // @return True if successful, if instrument found.
  Status SetNoteOn(int64 instrument_id, int64 timestamp, float note_pitch,
                   float note_intensity);

  // Sets note on callback.
  //
  // @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback note_on_callback);

  // Sets parameter value.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Timestamp in frames.
  // @param param_id Parameter id.
  // @param param_value Parameter value.
  // @return True if successful, if instrument parameter found.
  Status SetParam(int64 instrument_id, int64 timestamp, int param_id,
                  float param_value);

 private:
  // Sets instrument processor data.
  void SetProcessorData(int64 instrument_id, int64 timestamp,
                        InstrumentData data);

  // List of instruments.
  std::unordered_map<int64, InstrumentController> controllers_;
  std::unordered_map<int64, InstrumentProcessor> processors_;

  // int64 counter.
  int64 id_counter_;

  // Task runner.
  TaskRunner task_runner_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_MANAGER_H_
