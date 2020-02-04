#ifndef MUSICIAN_ENGINE_PERFORMER_H_
#define MUSICIAN_ENGINE_PERFORMER_H_

#include <functional>
#include <memory>
#include <unordered_set>

#include "barelymusician/engine/message_queue.h"
#include "barelymusician/instrument/instrument.h"

namespace barelyapi {

// Performer that processes an instrument with respect to incoming messages.
class Performer {
 public:
  // Note off callback signature.
  using NoteOffCallback = std::function<void(float index)>;

  // Note on callback signature.
  using NoteOnCallback = std::function<void(float index, float intensity)>;

  // Constructs new |Performer|.
  //
  // @param instrument Instrument to perform.
  explicit Performer(std::unique_ptr<Instrument> instrument);

  // Stops all scheduled notes that are being played.
  void AllScheduledNotesOff();

  // Stops playing note.
  //
  // @param index Note index.
  void NoteOff(float index);

  // Starts playing note.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  void NoteOn(float index, float intensity);

  // Processes the next output buffer for the given interval.
  //
  // @param start_position Start position to perform.
  // @param end_position End position to perform.
  // @param output Pointer to the output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void Process(double start_position, double end_position, float* output,
               int num_channels, int num_frames);

  // Schedules note off to be performed.
  //
  // @param index Note index.
  // @param position Position to perform note off.
  void ScheduleNoteOff(float index, double position);

  // Schedules note on to be performed.
  //
  // @param index Note index.
  // @param index Note intensity.
  // @param position Position to perform note on.
  void ScheduleNoteOn(float index, float intensity, double position);

  // Sets note off callback.
  //
  // @param note_off_callback Note off callback.
  void SetNoteOffCallback(NoteOffCallback&& note_off_callback);

  // Sets note on callback.
  //
  // @param note_on_callback Note on callback.
  void SetNoteOnCallback(NoteOnCallback&& note_on_callback);

 private:
  // Instrument to perform.
  std::unique_ptr<Instrument> instrument_;

  // Messages to perform.
  MessageQueue messages_;

  // Note off callback.
  NoteOffCallback note_off_callback_;

  // Note on callback.
  NoteOnCallback note_on_callback_;

  // Active scheduled note indices that are being played.
  std::unordered_set<float> scheduled_note_indices_;
};

}  // namespace barelyapi

#endif  // MUSICIAN_ENGINE_PERFORMER_H_