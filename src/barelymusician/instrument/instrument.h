#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

#include "barelymusician/message/message.h"
#include "barelymusician/message/message_buffer.h"

namespace barelyapi {

// Generic instrument interface.
class Instrument {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Instrument() = default;

  // Stops all notes.
  virtual void AllNotesOff() = 0;

  // Stops note with the given |index|.
  //
  // @param index Note index.
  virtual void NoteOff(float index) = 0;

  // Starts note with the given |index| and |intensity|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  virtual void NoteOn(float index, float intensity) = 0;

  // Processes the next |output| buffer.
  //
  // @param output Pointer to the output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  virtual void Process(float* output, int num_channels, int num_frames) = 0;

  // Stops note at the given |timestamp|.
  //
  // @param index Note index.
  // @param timestamp Note off timestamp.
  void NoteOffScheduled(float index, int timestamp);

  // Starts note at the given |timestamp|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  // @param timestamp Note on timestamp.
  void NoteOnScheduled(float index, float intensity, int timestamp);

  // Processes the next |output| buffer with the given |timestamp|.
  //
  // @param output Output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @param timestamp Start timestamp.
  void ProcessScheduled(float* output, int num_channels, int num_frames,
                        int timestamp);

 private:
  // Processes the given |message|.
  void ProcessMessage(const Message& message);

  // Messages to process.
  MessageBuffer message_buffer_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
