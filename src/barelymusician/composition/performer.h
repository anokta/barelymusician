#ifndef BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
#define BARELYMUSICIAN_COMPOSITION_PERFORMER_H_

#include <functional>
#include <list>
#include <memory>

#include "barelymusician/base/event.h"
#include "barelymusician/composition/message.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

class Performer {
 public:
  // Note off event callback signature.
  using NoteOffCallback = Event<float>::Callback;

  // Note on event callback signature.
  using NoteOnCallback = Event<float, float>::Callback;

  // Constructs new |Performer| with the given |instrument|.
  //
  // @param instrument Instrument to perform.
  explicit Performer(std::unique_ptr<Instrument> instrument);

  // Clears all notes.
  void Clear();

  // Stops note with the given |index|.
  //
  // @param index Note index.
  // @param start_sample Relative timestamp to stop the note.
  void NoteOff(float index, int timestamp);

  // Starts note with the given |index| and |intensity|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  // @param timestamp Relative timestamp to start the note.
  void NoteOn(float index, float intensity, int timestamp);

  // Processes the next |output| buffer.
  //
  // @param output Output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(float* output, int num_channels, int num_frames);

  // Registers note off callback.
  //
  // @param note_off_callback Note off callback for each note off.
  void RegisterNoteOffCallback(NoteOffCallback&& note_off_callback);

  // Registers note on callback.
  //
  // @param note_on_callback Note on callback for each note on.
  void RegisterNoteOnCallback(NoteOnCallback&& note_on_callback);

 private:
  // Processes the given note |message|.
  void ProcessMessage(const Message& message);

  // Pushes the given note |message| to be played.
  void PushMessage(const Message& message);

  // Instrument to perform.
  std::unique_ptr<Instrument> instrument_;

  // Event to be triggered for each note off.
  Event<float> note_off_event_;

  // Event to be triggered for each note on.
  Event<float, float> note_on_event_;

  // Note messages to play.
  std::list<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
