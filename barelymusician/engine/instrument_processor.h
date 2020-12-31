#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <utility>
#include <vector>

#include "barelymusician/engine/instrument_definition.h"
#include "barelymusician/engine/message_data.h"

namespace barelyapi {

class InstrumentProcessor {
 public:
  InstrumentProcessor(int sample_rate, InstrumentDefinition definition);
  ~InstrumentProcessor();

  // Non-copyable.
  InstrumentProcessor(const InstrumentProcessor& other) = delete;
  InstrumentProcessor& operator=(const InstrumentProcessor& other) = delete;

  // Movable.
  InstrumentProcessor(InstrumentProcessor&& other) noexcept;
  InstrumentProcessor& operator=(InstrumentProcessor&& other) noexcept;

  void Create();
  void Destroy();

  void Process(double timestamp, float* output, int num_channels,
               int num_frames);

  void ScheduleNoteOff(double timestamp, float note_index);
  void ScheduleNoteOn(double timestamp, float note_index, float note_intensity);
  void ScheduleParam(double timestamp, int param_id, float param_value);

  void SetCustomData(void* data);

  void SetNoteOff(float note_index);
  void SetNoteOn(float note_index, float note_intensity);
  void SetParam(int param_id, float param_value);

 private:
  int sample_rate_;
  InstrumentDefinition definition_;
  InstrumentState state_;
  std::vector<std::pair<double, MessageData>> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
