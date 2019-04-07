#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PLAYER_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PLAYER_H_

#include <list>

#include "barelymusician/base/module.h"
#include "barelymusician/base/types.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message.h"

namespace barelyapi {

// Class that plays a given instrument by sample accurate input directives.
class InstrumentPlayer : public Module {
 public:
  // Constructs new |InstrumentPlayer| with the given |instrument|.
  //
  // @param instrument Instrument to play.
  explicit InstrumentPlayer(Instrument* instrument);

  // Implements |Module|.
  void Reset() override;

  // Plays note with the given |sample_offset|.
  //
  // @param sample_offset Sample offset to play the note.
  // @param index Note index.
  // @param intensity Note intensity.
  void PlayNote(int sample_offset, float index, float intensity);

  // Stops note with the given |sample_offset|.
  //
  // @param timestamp Timestamp to stop the note.
  // @param index Note index.
  void StopNote(int sample_offset, float index);

  // Updates float parameter with the given |sample_offset|.
  //
  // @param sample_offset Sample offset to update the parameter.
  // @param id Parameter ID.
  // @param value Parameter value.
  void UpdateFloatParam(int sample_offset, ParamId id, float value);

  // Processes the next output samples.
  // TODO(#20): Create and pass an AudioBuffer here instead?
  //
  // @param num_samples Number of samples to process.
  // @param output Pointer to output samples to be written into.
  void Process(int num_samples, float* output);

 private:
  // Processes the given |message|.
  void ProcessMessage(const Message& message);

  // Enqueues the given |message| to be processed.
  void PushMessage(const Message& message);

  // Instrument to be played.
  Instrument* instrument_;  // not owned.

  // List of messages that stores instrument input directives.
  std::list<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_PLAYER_H_