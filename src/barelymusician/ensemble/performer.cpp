#include "barelymusician/ensemble/performer.h"

#include <algorithm>
#include <utility>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const int kNoteOnId = 0;
const int kNoteOffId = 1;
const int kSetFloatParamId = 2;

// |NoteOn| message data.
struct NoteOnData {
  float index;
  float intensity;
};

// |NoteOff| message data.
struct NoteOffData {
  float index;
};

// |SetFloatParam| message data.
struct SetFloatParamData {
  int id;
  float value;
};

}  // namespace

Performer::Performer(Instrument* instrument) : instrument_(instrument) {
  DCHECK(instrument_);
}

void Performer::Reset() {
  messages_.clear();
  instrument_->Reset();
}

void Performer::NoteOn(int start_sample, float index, float intensity) {
  PushMessage(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, start_sample));
}

void Performer::NoteOff(int start_sample, float index) {
  PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index}, start_sample));
}

void Performer::PlayNote(int start_sample, int duration_samples, float index,
                         float intensity) {
  NoteOn(start_sample, index, intensity);
  NoteOff(start_sample + duration_samples, index);
}

void Performer::Process(int num_samples, float* output) {
  DCHECK(output);
  int i = 0;
  // Process samples within message events range.
  const auto message_begin = std::lower_bound(
      messages_.begin(), messages_.end(), 0, &CompareTimestamp);
  if (message_begin != messages_.end()) {
    const auto message_end = std::lower_bound(message_begin, messages_.end(),
                                              num_samples, &CompareTimestamp);
    for (auto it = message_begin; it != message_end; ++it) {
      while (i < it->timestamp) {
        output[i++] = instrument_->Next();
      }
      ProcessMessage(*it);
    }
    messages_.erase(message_begin, message_end);
  }
  // Process remaining samples.
  while (i < num_samples) {
    output[i++] = instrument_->Next();
  }
  // Update message timestamps.
  for (auto& message : messages_) {
    message.timestamp -= num_samples;
  }
}

void Performer::SetFloatParam(int start_sample, int id, float value) {
  PushMessage(BuildMessage<SetFloatParamData>(kSetFloatParamId, {id, value},
                                              start_sample));
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
    case kSetFloatParamId: {
      const auto set_float_param =
          ReadMessageData<SetFloatParamData>(message.data);
      instrument_->SetFloatParam(set_float_param.id, set_float_param.value);
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
