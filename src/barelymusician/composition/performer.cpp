#include "barelymusician/composition/performer.h"

#include "barelymusician/base/logging.h"
#include "barelymusician/base/note.h"
#include "barelymusician/base/param.h"
#include "barelymusician/message/message.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

PerformType PerformTypeFromMessageId(MessageId id) {
  return static_cast<PerformType>(id);
}

}  // namespace

Performer::Performer(Instrument* instrument) : instrument_(instrument) {
  DCHECK(instrument_);
}

void Performer::Perform(const Message& message) {
  message_queue_.Push(message);
}

void Performer::Process(int timestamp, int num_samples, float* output) {
  DCHECK(output);

  int i = 0;
  // Process samples within message events range.
  const auto messages = message_queue_.Pop(timestamp, num_samples);
  for (const auto& message : messages) {
    while (i + timestamp < message.timestamp) {
      // TODO(#21): Note that, this won't handle any messages with a timestamp
      // prior to the input |timestamp| *properly*, they will be triggered
      // immediately at the beginning of the buffer.
      output[i++] = instrument_->Next();
    }
    // Perform the message.
    switch (PerformTypeFromMessageId(message.id)) {
      case PerformType::kNoteOn: {
        const auto note_on = ReadMessageData<Note>(message.data);
        instrument_->NoteOn(note_on.index, note_on.intensity);
      } break;
      case PerformType::kNoteOff: {
        const auto note_off = ReadMessageData<Note>(message.data);
        instrument_->NoteOff(note_off.index);
      } break;
      case PerformType::kFloatParam: {
        const auto float_param = ReadMessageData<Param<float>>(message.data);
        if (!instrument_->SetFloatParam(float_param.id, float_param.value)) {
          LOG(WARNING) << "Failed to set float param with ID: "
                       << float_param.id;
        }
      } break;
      default:
        LOG(ERROR) << "Unknown message ID: " << message.id;
        break;
    }
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
