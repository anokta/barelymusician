#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <utility>
#include <vector>

#include "barelymusician/base/types.h"
#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/message_data.h"

namespace barelyapi {

class InstrumentProcessor {
 public:
  InstrumentProcessor(InstrumentDefinition definition);
  ~InstrumentProcessor();

  // Non-copyable.
  InstrumentProcessor(const InstrumentProcessor& other) = delete;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = delete;

  // Movable.
  InstrumentProcessor(InstrumentProcessor&& other) noexcept;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) noexcept;

  void Process(int64 timestamp, float* output, int num_channels,
               int num_frames);

  void ScheduleNoteOff(int64 timestamp, float note_index);
  void ScheduleNoteOn(int64 timestamp, float note_index, float note_intensity);

  void SetCustomData(void* data);

  void SetNoteOff(float note_index);
  void SetNoteOn(float note_index, float note_intensity);
  void SetParam(int param_id, float param_value);

 private:
  InstrumentDefinition definition_;
  InstrumentState state_;
  std::vector<std::pair<int64, MessageData>> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
