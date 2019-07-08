#ifndef BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
#define BARELYMUSICIAN_COMPOSITION_PERFORMER_H_

#include <memory>

#include "barelymusician/instrument/instrument.h"
#include "barelymusician/message/message.h"
#include "barelymusician/message/message_buffer.h"

namespace barelyapi {

// Class that performs an instrument in a sample accurate manner.
class Performer {
 public:
  // Constructs new |Performer|.
  //
  // @param instrument Instrument to perform.
  explicit Performer(std::unique_ptr<Instrument> instrument);

  // Returns the mutable instrument.
  Instrument* GetInstrument() const;

  // Processes the next |output| buffer with the given |timestamp|.
  //
  // @param output Output buffer.
  // @param num_channels Number of output channels.
  // @param num_frames Number of output frames.
  // @param timestamp Start timestamp.
  void Process(float* output, int num_channels, int num_frames, int timestamp);

  // Starts playing instrument note.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  // @param timestamp Note on timestamp.
  void StartNote(float index, float intensity, int timestamp);

  // Stops playing instrument note.
  //
  // @param index Note index.
  // @param timestamp Note off timestamp.
  void StopNote(float index, int timestamp);

 private:
  // Processes the given |message|.
  void ProcessMessage(const Message& message);

  // Instrument to perform.
  std::unique_ptr<Instrument> instrument_;

  // Messages to perform.
  MessageBuffer message_buffer_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_COMPOSITION_PERFORMER_H_
