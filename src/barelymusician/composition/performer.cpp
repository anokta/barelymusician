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

void Performer::Reset() {
  messages_.clear();
  instrument_->Reset();
}

void Performer::Perform(const std::vector<Note>& notes, int beat_start_sample,
                        int num_samples_per_beat) {
  for (const Note& note : notes) {
    const float index = note.index;
    const float intensity = note.intensity;
    const int start_sample =
        beat_start_sample +
        static_cast<int>(note.start_beat *
                         static_cast<float>(num_samples_per_beat));
    PushMessage(
        BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, start_sample));
    const int end_sample =
        start_sample +
        static_cast<int>(note.duration_beats *
                         static_cast<float>(num_samples_per_beat));
    PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index}, end_sample));
  }
}

void Performer::Process(float* output, int num_channels, int num_frames) {
  // Process frames within message events range.
  int frame = 0;
  const auto begin = messages_.begin();
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
