#ifndef BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
#define BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_

#include <memory>
#include <unordered_map>

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
  // @param instrument Instrument to process.
  // @return Instrument id.
  int Create(std::unique_ptr<Instrument> instrument);

  // Destroys instrument.
  //
  // @param instrument_id Instrument id.
  void Destroy(int instrument_id);

  // Stops playing note.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param index Note index.
  void NoteOff(int instrument_id, double timestamp, float index);

  // Starts playing note.
  //
  // @param instrument_id Instrument id.
  // @param timestamp Note timestamp.
  // @param index Note index.
  // @param intensity Note intensity.
  void NoteOn(int instrument_id, double timestamp, float index,
              float intensity);

  // Processes the next output buffer.
  //
  // @param instrument_id Instrument id.
  // @param begin_timestamp Begin timestamp.
  // @param begin_timestamp End timestamp.
  // @param output Pointer to output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(int instrument_id, double begin_timestamp, double end_timestamp,
               float* output, int num_channels, int num_frames);

 private:
  // Instrument data.
  struct InstrumentData {
    // Instrument to play.
    std::unique_ptr<Instrument> instrument;

    // Scheduled messages.
    MessageQueue messages;
  };

  // List of instruments.
  std::unordered_map<int, InstrumentData> instruments_;

  // Id counter.
  int id_counter_;

  // Task runner.
  TaskRunner task_runner_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENGINE_INSTRUMENT_MANAGER_H_
