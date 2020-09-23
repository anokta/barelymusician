#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

#include "barelymusician/engine/message_queue.h"

namespace barelyapi {

// Generic instrument interface.
class Instrument {
 public:
  // Base destructor to ensure the derived classes get destroyed properly.
  virtual ~Instrument() = default;

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

  // Sets |value| of parameter with the given |id|.
  //
  // @param id Parameter id.
  // @param value Float parameter value.
  virtual void SetParam(int id, float value) = 0;

  // Stops note with the given |index| at the given |timestamp|.
  //
  // @param timestamp Timestamp to stop note.
  // @param index Note index.
  void NoteOffScheduled(double timestamp, float index);

  // Starts note with the given |index| and |intensity| at the given
  // |timestamp|.
  //
  // @param timestamp Timestamp to start note.
  // @param index Note index.
  // @param intensity Note intensity.
  void NoteOnScheduled(double timestamp, float index, float intensity);

  // Processes the next |output| buffer at the given |begin_timestamp| and
  // |end_timestamp|.
  //
  // @param begin_timestamp Begin timestamp.
  // @param begin_timestamp End timestamp.
  // @param output Pointer to the output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  void ProcessScheduled(double begin_timestamp, double end_timestamp,
                        float* output, int num_channels, int num_frames);

 private:
  // Scheduled messages to process.
  MessageQueue messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
