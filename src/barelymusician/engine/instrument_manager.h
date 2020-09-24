#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <memory>
#include <unordered_map>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/message_queue.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/util/task_runner.h"

namespace barelyapi {

// Class that manages processing of instruments.
class InstrumentManager {
 public:
  // Constructs new |InstrumentManager|.
  InstrumentManager();

  // Creates new instrument.
  //
  // @param definition Instrument definition.
  // @param arguments Instrument arguments.
  // @return Instrument id.
  template <typename InstrumentType, typename... ArgumentTypes>
  int Create(const InstrumentDefinition& definition,
             ArgumentTypes... arguments);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  bool Destroy(int instrument_id);

  // Returns parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @param value Parameter value.
  bool GetParam(int instrument_id, int param_id, float* value) const;

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  bool NoteOff(int instrument_id, float index);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param index Note index.
  // @param intensity Note intensity.
  bool NoteOn(int instrument_id, float index, float intensity);

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param begin_timestamp End timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  bool Process(int instrument_id, double begin_timestamp, double end_timestamp,
               float* output, int num_channels, int num_frames);

  // Sets parameter value.
  //
  // @param instrument_id Instrument id.
  // @param param_id Parameter id.
  // @param value Parameter value.
  bool SetParam(int instrument_id, int param_id, float value);

  // Schedules note off.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param index Note index.
  bool ScheduleNoteOff(int instrument_id, double timestamp, float index);

  // Schedules note on.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param index Note index.
  // @param index Note intensity.
  bool ScheduleNoteOn(int instrument_id, double timestamp, float index,
                      float intensity);

 private:
  // Instrument data.
  struct InstrumentData {
    // Instrument to play.
    std::unique_ptr<Instrument> instrument;

    // Scheduled messages.
    MessageQueue messages;
  };

  // Instrument parameter data.
  struct InstrumentParamData {
    // Instrument definition.
    InstrumentDefinition definition;

    // Instrument param values.
    std::unordered_map<int, float> values;
  };

  // List of instruments.
  std::unordered_map<int, InstrumentData> instruments_;

  // List of instrument params.
  std::unordered_map<int, InstrumentParamData> instrument_params_;

  // Id counter.
  int id_counter_;

  // Task runner.
  TaskRunner task_runner_;
};

template <typename InstrumentType, typename... ArgumentTypes>
int InstrumentManager::Create(const InstrumentDefinition& definition,
                              ArgumentTypes... arguments) {
  const int instrument_id = ++id_counter_;
  auto it = instrument_params_.emplace(instrument_id,
                                       InstrumentParamData{definition});
  auto& instrument_param_data = it.first->second;
  for (const auto& param : instrument_param_data.definition.params) {
    instrument_param_data.values.emplace(param.id, param.default_value);
  }
  task_runner_.Add([this, instrument_id, params = definition.params,
                    arguments...]() {
    auto instrument = std::make_unique<InstrumentType>(arguments...);
    for (const auto& param : params) {
      instrument->SetParam(param.id, param.default_value);
    }
    instruments_.emplace(instrument_id, InstrumentData{std::move(instrument)});
  });
  return instrument_id;
}

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
