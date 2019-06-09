#include "barelymusician/composition/performer.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/composition/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const int kNoteOffId = 0;
const int kNoteOnId = 1;

// |NoteOff| message data.
struct NoteOffData {
  float index;
};

// |NoteOn| message data.
struct NoteOnData {
  float index;
  float intensity;
};

}  // namespace

Performer::Performer(std::unique_ptr<Instrument> instrument)
    : instrument_(std::move(instrument)) {
  DCHECK(instrument_);
}

void Performer::ClearAllNotes() {
  messages_.clear();
  instrument_->Clear();
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

void Performer::RegisterNoteOffCallback(NoteOffCallback&& note_off_callback) {
  note_off_event_.Register(std::move(note_off_callback));
}

void Performer::RegisterNoteOnCallback(NoteOnCallback&& note_on_callback) {
  note_on_event_.Register(std::move(note_on_callback));
}

void Performer::StartNote(float index, float intensity, int offset_samples) {
  PushMessage(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, offset_samples));
}

void Performer::StopNote(float index, int offset_samples) {
  PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index}, offset_samples));
}

void Performer::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kNoteOffId: {
      const auto note_off = ReadMessageData<NoteOffData>(message.data);
      instrument_->NoteOff(note_off.index);
      note_off_event_.Trigger(note_off.index);
    } break;
    case kNoteOnId: {
      const auto note_on = ReadMessageData<NoteOnData>(message.data);
      instrument_->NoteOn(note_on.index, note_on.intensity);
      note_on_event_.Trigger(note_on.index, note_on.intensity);
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
