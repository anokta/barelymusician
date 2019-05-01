#include "barelymusician/instrument/instrument.h"

#include <algorithm>
#include <iterator>

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

Instrument::Instrument(int num_channels) : num_channels_(num_channels) {
  DCHECK_GE(num_channels, 0);
}

void Instrument::Reset() {
  messages_.clear();
  Clear();
}

int Instrument::GetNumChannels() const { return num_channels_; }

void Instrument::Process(Buffer* output) {
  DCHECK(output);
  DCHECK_EQ(num_channels_, output->num_channels());

  const int num_frames = output->num_frames();
  // Process frames within message events range.
  int i = 0;
  const auto begin = messages_.begin();
  const auto end =
      std::lower_bound(begin, messages_.end(), num_frames, &CompareTimestamp);
  if (begin != end) {
    for (auto it = begin; it != end; ++it) {
      while (i < it->timestamp) {
        ProcessFrame(&(*output)[i++]);
      }
      ProcessMessage(*it);
    }
    messages_.erase(begin, end);
  }
  // Process remaining frames.
  while (i < num_frames) {
    ProcessFrame(&(*output)[i++]);
  }

  // Update message timestamps.
  for (auto& message : messages_) {
    message.timestamp -= num_frames;
  }
}

void Instrument::StartNote(float index, float intensity, int sample_offset) {
  PushMessage(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, sample_offset));
}

void Instrument::StopNote(float index, int sample_offset) {
  PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index}, sample_offset));
}

void Instrument::ProcessFrame(Frame* frame) {
  for (int channel = 0; channel < num_channels_; ++channel) {
    (*frame)[channel] = Next(channel);
  }
}

void Instrument::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kNoteOnId: {
      const auto note_on = ReadMessageData<NoteOnData>(message.data);
      NoteOn(note_on.index, note_on.intensity);
    } break;
    case kNoteOffId: {
      const auto note_off = ReadMessageData<NoteOffData>(message.data);
      NoteOff(note_off.index);
    } break;
    default:
      DLOG(ERROR) << "Unknown message ID: " << message.id;
      break;
  }
}

void Instrument::PushMessage(const Message& message) {
  DCHECK_GE(message.timestamp, 0);
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
