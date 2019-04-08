#include "barelymusician/instrument/instrument_processor.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const MessageId kNoteOnId = 0;
const MessageId kNoteOffId = 1;
const MessageId kSetFloatParamId = 2;

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
  ParamId id;
  float value;
};

// Returns new |Message| with the given message |id|, |data| and |timestamp|.
//
// @param id Message ID.
// @param data Message data.
// @param timestamp Message timestamp.
// @return Message.
template <typename DataType>
Message BuildMessage(MessageId id, const DataType& data, int timestamp) {
  Message message;
  message.id = id;
  WriteMessageData<DataType>(data, message.data);
  message.timestamp = timestamp;
  return message;
}

// Compares the given two messages with respect to their timestamps.
//
// @param lhs First message.
// @param rhs Second message.
// @return True if the first message comes prior to the second message.
bool CompareMessage(const Message& lhs, const Message& rhs) {
  return lhs.timestamp < rhs.timestamp;
}

// Compares the given |message| against the given |timestamp|.
//
// @param message Message.
// @param timestamp Timestamp.
// @return True if the message comes prior to the timestamp.
bool CompareTimestamp(const Message& message, int timestamp) {
  return message.timestamp < timestamp;
}

}  // namespace

InstrumentProcessor::InstrumentProcessor(Instrument* instrument)
    : instrument_(instrument) {
  DCHECK(instrument_);
}

void InstrumentProcessor::Reset() {
  messages_.clear();
  instrument_->Reset();
}

void InstrumentProcessor::NoteOn(int sample_offset, float index,
                                 float intensity) {
  PushMessage(
      BuildMessage<NoteOnData>(kNoteOnId, {index, intensity}, sample_offset));
}

void InstrumentProcessor::NoteOff(int sample_offset, float index) {
  PushMessage(BuildMessage<NoteOffData>(kNoteOffId, {index}, sample_offset));
}

void InstrumentProcessor::SetFloatParam(int sample_offset, ParamId id,
                                        float value) {
  PushMessage(BuildMessage<SetFloatParamData>(kSetFloatParamId, {id, value},
                                              sample_offset));
}

void InstrumentProcessor::Process(int num_samples, float* output) {
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

void InstrumentProcessor::ProcessMessage(const Message& message) {
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

void InstrumentProcessor::PushMessage(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
