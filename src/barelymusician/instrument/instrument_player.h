#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PLAYER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PLAYER_H_

#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message_queue.h"

namespace barelyapi {

// Class that plays a given instrument by sample accurate input directives.
class InstrumentPlayer {
 public:
  // Constructs new |InstrumentPlayer| with the given |instrument|.
  //
  // @param instrument Instrument to play.
  explicit InstrumentPlayer(Instrument* instrument);

  // Plays note at the given |timestamp|.
  //
  // @param timestamp Timestamp to play the note.
  // @param index Note index.
  // @param intensity Note intensity.
  void PlayNote(int timestamp, float index, float intensity);

  // Stops note at the given |timestamp|.
  //
  // @param timestamp Timestamp to stop the note.
  // @param index Note index.
  void StopNote(int timestamp, float index);

  // Updates float parameter at the given |timestamp|.
  //
  // @param timestamp Timestamp to update the parameter.
  // @param id Parameter ID.
  // @param value Parameter value.
  void UpdateFloatParam(int timestamp, ParamId id, float value);

  // Processes the next output samples from the given |timestamp|.
  // TODO(#20): Create and pass an AudioBuffer here instead?
  //
  // @param timestamp Start sample to process.
  // @param num_samples Number of samples to process.
  // @param output Pointer to output samples to be written into.
  void Process(int timestamp, int num_samples, float* output);

  // Resets the player.
  void Reset();

 private:
  // Instrument to be played.
  Instrument* instrument_;  // not owned.

  // Message queue to schedule instrument input directives.
  MessageQueue message_queue_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PLAYER_H_
