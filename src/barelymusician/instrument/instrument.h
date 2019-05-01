#ifndef BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
#define BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_

#include <list>

#include "barelymusician/base/buffer.h"
#include "barelymusician/base/frame.h"
#include "barelymusician/base/module.h"
#include "barelymusician/message/message.h"

namespace barelyapi {

// Generic instrument interface.
class Instrument : public Module {
 public:
  // Constructs new |Instrument| with the given |num_channels|.
  //
  // @param num_channels Number of output channels.
  explicit Instrument(int num_channels);

  // Implements |Module|.
  void Reset() override;

  // Returns the number of output channels.
  //
  // @return Number of output channels.
  int GetNumChannels() const;

  // Processes the next output buffer.
  //
  // @param output Pointer to the output buffer.
  void Process(Buffer* output);

  // Starts playing a note with the given |sample_offset|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  // @param sample_offset Relative sample offset to start playing the note.
  void StartNote(float index, float intensity, int sample_offset = 0);

  // Stops playing a note with the given |sample_offset|.
  //
  // @param index Note index.
  // @param sample_offset Relative sample offset to stop playing the note.
  void StopNote(float index, int sample_offset = 0);

 protected:
  // Clears all notes.
  virtual void Clear() = 0;

  // Generates the next output sample for the given |channel|.
  //
  // @param Output sample.
  virtual float Next(int channel) = 0;

  // Stops note with the given |index|.
  //
  // @param index Note index.
  virtual void NoteOff(float index) = 0;

  // Starts note with the given |index| and |intensity|.
  //
  // @param index Note index.
  // @param intensity Note intensity.
  virtual void NoteOn(float index, float intensity) = 0;

 private:
  // Processes the given |frame|.
  void ProcessFrame(Frame* frame);

  // Processes the given note |message|.
  void ProcessMessage(const Message& notemessage);

  // Enqueues the given note |message| to be processed.
  void PushMessage(const Message& note_message);

  // Number of output channels.
  const int num_channels_;

  // Note messages to process.
  std::list<Message> messages_;
};

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_INSTRUMENT_INSTRUMENT_H_
