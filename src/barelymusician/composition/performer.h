#ifndef BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
#define BARELYMUSICIAN_COMPOSITION_PERFORMER_H_

#include <list>
#include <vector>

#include "barelymusician/base/module.h"
#include "barelymusician/composition/note.h"
#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message.h"

namespace barelyapi {

class Performer : public Module {
 public:
  // Constructs new |Performer| with the given |instrument|.
  //
  // @param instrument Pointer to the instrument to perform.
  explicit Performer(Instrument* instrument);

  // Implements |Module|.
  void Reset() override;

  // Stops note with the given |index|.
  //
  // @param index Note index.
  // @param timestamp Timestamp to stop the note.
  void NoteOff(float index, int timestamp);

  // Starts note with the given |index| and |intensity|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  // @param timestamp Timestamp to start the note.
  void NoteOn(float index, float intensity, int timestamp);

  // Processes the next |output| buffer.
  //
  // @param output Output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(float* output, int num_channels, int num_frames);

 private:
  // Processes the given note |message|.
  void ProcessMessage(const Message& message);

  // Pushes the given note |message| to be played.
  void PushMessage(const Message& message);

  // Instrument to perform.
  Instrument* const instrument_;  // not owned.

  // Note messages to play.
  std::list<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
