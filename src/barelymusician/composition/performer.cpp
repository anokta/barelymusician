#include "barelymusician/composition/performer.h"

#include <functional>
#include <unordered_map>

#include "barelymusician/base/logging.h"
#include "barelymusician/base/note.h"
#include "barelymusician/base/param.h"
#include "barelymusician/composition/message.h"
#include "barelymusician/composition/message_utils.h"

namespace barelyapi {

namespace {

// Type-agnostic message performer signutare.
using PerformerFn = std::function<void(const unsigned char*, Instrument*)>;

// Performs note on event on the given |instrument|.
//
// @param message_data Message data that contains the note on event.
// @param instrument Pointer to instrument.
void PerformNoteOn(const unsigned char* message_data, Instrument* instrument) {
  const auto note = ReadMessageData<Note>(message_data);
  instrument->NoteOn(note.index, note.intensity);
}

// Performs note off event on the given |instrument|.
//
// @param message_data Message data that contains the note off event.
// @param instrument Pointer to instrument.
void PerformNoteOff(const unsigned char* message_data, Instrument* instrument) {
  const auto note = ReadMessageData<Note>(message_data);
  instrument->NoteOff(note.index);
}

// Performs float parameter event on the given |instrument|.
//
// @param message_data Message data that contains the float parameter event.
// @param instrument Pointer to instrument.
void PerformFloatParam(const unsigned char* message_data,
                       Instrument* instrument) {
  const auto float_param = ReadMessageData<Param<float>>(message_data);
  instrument->SetFloatParam(float_param.id, float_param.value);
}

static const std::unordered_map<MessageType, PerformerFn> kPerformerMap = {
    {MessageType::kNoteOn, &PerformNoteOn},
    {MessageType::kNoteOff, &PerformNoteOff},
    {MessageType::kFloatParam, &PerformFloatParam}};

}  // namespace

Performer::Performer(Instrument* instrument) : instrument_(instrument) {
  DCHECK(instrument_);
}

void Performer::Perform(const Message& message) {
  message_queue_.Push(message);
}

void Performer::Process(int timestamp, int num_samples, float* output) {
  int i = 0;
  // Process samples within message events range.
  const auto messages = message_queue_.Pop(timestamp, num_samples);
  for (const auto& message : messages) {
    while (i + timestamp < message.timestamp) {
      // TODO(#21): Note that, this won't handle any returned notes with a
      // timestamp prior to the input |timestamp| *properly*, they will be
      // triggered immediately at the beginning of the buffer.
      output[i++] = instrument_->Next();
    }
    // Perform the message.
    DCHECK(kPerformerMap.find(message.type) != kPerformerMap.end());
    kPerformerMap.at(message.type)(message.data, instrument_);
  }
  // Process remaining samples.
  while (i < num_samples) {
    output[i++] = instrument_->Next();
  }
}

void Performer::Reset() {
  message_queue_.Reset();
  instrument_->Reset();
}

}  // namespace barelyapi
