#include "barelymusician/composition/performer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const int kNoteOnId = 0;
const int kNoteOffId = 1;

// |NoteOn| message data.
struct NoteOnData {
  float index;
  float intensity;
};

// |NoteOff| message data.
struct NoteOffData {
  float index;
};

}  // namespace

Performer::Performer(Instrument* instrument) : instrument_(instrument) {
  DCHECK(instrument_);
}

void Performer::Clear() {
  messages_.clear();
  instrument_->Clear();
}

void Performer::NoteOn(float index, float intensity, int timestamp) {
  PushMessage(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, timestamp));
}

void Performer::NoteOff(float index, int timestamp) {
  PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index}, timestamp));
}

void Performer::Process(float* output, int num_channels, int num_frames) {
  // Process frames within message events range.
  int frame = 0;
  const auto begin = std::lower_bound(messages_.begin(), messages_.end(), 0,
                                      &CompareTimestamp);
  const auto end =
      std::lower_bound(begin, messages_.end(), num_frames, &CompareTimestamp);
  if (begin != end) {
    for (auto it = begin; it != end; ++it) {
      if (frame < it->timestamp) {
        instrument_->Process(&output[frame * num_channels], num_channels,
                             it->timestamp - frame);
        frame = it->timestamp;
      }
      ProcessMessage(*it);
    }
    messages_.erase(begin, end);
  }
  // Process remaining frames.
  if (frame < num_frames) {
    instrument_->Process(&output[frame * num_channels], num_channels,
                         num_frames - frame);
  }
  // Update message timestamps.
  for (Message& message : messages_) {
    message.timestamp -= num_frames;
  }
}

void Performer::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kNoteOnId: {
      const auto note_on = ReadMessageData<NoteOnData>(message.data);
      instrument_->NoteOn(note_on.index, note_on.intensity);
    } break;
    case kNoteOffId: {
      const auto note_off = ReadMessageData<NoteOffData>(message.data);
      instrument_->NoteOff(note_off.index);
    } break;
    default:
      DLOG(ERROR) << "Unknown message ID: " << message.id;
      break;
  }
}

void Performer::PushMessage(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
