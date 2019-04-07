#include "barelymusician/instrument/instrument_player.h"

#include <algorithm>

#include "barelymusician/base/logging.h"
#include "barelymusician/message/message_utils.h"

namespace barelyapi {

namespace {

// Unique message IDs per message type.
const MessageId kPlayNoteId = 0;
const MessageId kStopNoteId = 1;
const MessageId kUpdateFloatParamId = 2;

// Play note message data.
struct PlayNoteData {
  float index;
  float intensity;
};

// Stop note message data.
struct StopNoteData {
  float index;
};

// Update float parameter message data.
struct UpdateFloatParamData {
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

InstrumentPlayer::InstrumentPlayer(Instrument* instrument)
    : instrument_(instrument) {
  DCHECK(instrument_);
}

void InstrumentPlayer::Reset() {
  messages_.clear();
  instrument_->Reset();
}

void InstrumentPlayer::PlayNote(int sample_offset, float index,
                                float intensity) {
  PushMessage(BuildMessage<PlayNoteData>(kPlayNoteId, {index, intensity},
                                         sample_offset));
}

void InstrumentPlayer::StopNote(int sample_offset, float index) {
  PushMessage(BuildMessage<StopNoteData>(kStopNoteId, {index}, sample_offset));
}

void InstrumentPlayer::UpdateFloatParam(int sample_offset, ParamId id,
                                        float value) {
  PushMessage(BuildMessage<UpdateFloatParamData>(kUpdateFloatParamId,
                                                 {id, value}, sample_offset));
}

void InstrumentPlayer::Process(int num_samples, float* output) {
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

void InstrumentPlayer::ProcessMessage(const Message& message) {
  switch (message.id) {
    case kPlayNoteId: {
      const auto play_note = ReadMessageData<PlayNoteData>(message.data);
      instrument_->NoteOn(play_note.index, play_note.intensity);
    } break;
    case kStopNoteId: {
      const auto stop_note = ReadMessageData<StopNoteData>(message.data);
      instrument_->NoteOff(stop_note.index);
    } break;
    case kUpdateFloatParamId: {
      const auto update_float_param =
          ReadMessageData<UpdateFloatParamData>(message.data);
      instrument_->SetFloatParam(update_float_param.id,
                                 update_float_param.value);
    } break;
    default:
      DLOG(ERROR) << "Unknown message ID: " << message.id;
      break;
  }
}

void InstrumentPlayer::PushMessage(const Message& message) {
  const auto it = std::upper_bound(messages_.begin(), messages_.end(), message,
                                   &CompareMessage);
  messages_.insert(it, message);
}

}  // namespace barelyapi
