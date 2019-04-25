#ifndef BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_
#define BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_

#include <list>

#include "barelymusician/base/module.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message.h"

namespace barelyapi {

// Instrument player that performs sample accurate input directives.
class Performer : public Module {
 public:
  // Constructs new |Performer| with the given |instrument|.
  //
  // @param instrument Instrument to play.
  explicit Performer(Instrument* instrument);

  // Implements |Module|.
  void Reset() override;

  // Stops playing note at the given |start_sample|.
  //
  // @param start_sample Relative sample offset to stop playing the note.
  // @param index Note index.
  void NoteOff(int start_sample, float index);

  // Starts playing note at the given |start_sample|.
  //
  // @param start_sample Relative sample offset to start playing the note.
  // @param index Note index.
  // @param intensity Note intensity.
  void NoteOn(int start_sample, float index, float intensity);

  // Plays note at the given |start_sample| for |duration_samples|.
  //
  // @param start_sample Relative sample offset to play the note.
  // @param duration_samples Play duration of the note in samples.
  // @param index Note index.
  // @param intensity Note intensity.
  void PlayNote(int start_sample, int duration_samples, float index,
                float intensity);

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

  // Instrument to be processed.
  Instrument* instrument_;  // not owned.

  // List of messages that stores instrument input directives.
  std::list<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_ENSEMBLE_PERFORMER_H_
