#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "barelymusician/engine/instrument.h"
#include "barelymusician/engine/message_queue.h"

namespace barelyapi {

// Instrument processor.
class InstrumentProcessor {
 public:
  // Constructs new |InstrumentProcessor|.
  //
  // @param instrument Instrument to process.
  // @param params Default instrument params.
  InstrumentProcessor(std::unique_ptr<Instrument> instrument,
                      const std::vector<std::pair<int, float>>& params);

  // Stops all notes.
  void AllNotesOff();

  // TODO: refactor?
  void Message(double timestamp, Message::Data data);

  // Stops note with the given |index|.
  //
  // @param index Note index.
  void NoteOff(double timestamp, float index);

  // Starts note with the given |index| and |intensity|.
  //
  // @param index Note index.
  void NoteOn(double timestamp, float index, float intensity);

  // Processes the next output buffer.
  //
  // @param begin_timestamp Begin timestamp.
  // @param end_timestamp Begin timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(double begin_timestamp, double end_timestamp, float* output,
               int num_channels, int num_frames);

  void SetParam(double timestamp, int id, float value);

 private:
  // Instrument to process.
  std::unique_ptr<Instrument> instrument_;

  // Messages to process.
  MessageQueue messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_PROCESSOR_H_
